module;
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include "EngineMacro.h"

module VT.Platform.Vulkan.Context;
import VT.Log;
import VT.Util;

#define VK_CHECK(res, expect, ...)              \
    VT_CORE_ASSERT(                             \
        static_cast<vk::Result>(res) == expect, \
        "Vulkan check failed at ",              \
        __FILE__,                               \
        __LINE__,                               \
        " : ",                                  \
        __VA_ARGS__);

namespace VT::Vulkan
{
Context::Context(Shared<Window> Window) : m_Window(Window) {}

void Context::Init()
{
    VT_CORE_TRACE("Vulkan context init");
    /* ===============================================
     *          Create Vulkan Instance
     * ===============================================
     */
    {
        std::vector<const char*> InstanceExtension;
        std::vector<const char*> InstanceLayer;

        switch (m_Window->GetWindowAPI())
        {
            case WindowAPI::eGLFWwindow:
            {
                VT_CORE_ASSERT(glfwVulkanSupported(), "GLFW Window context does not support vulkan");
                uint32_t GlfwExtensionCount;
                const char** GlfwExtension = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

                // append to extension list
                InstanceExtension.reserve(GlfwExtensionCount);
                InstanceExtension.insert(InstanceExtension.cend(), GlfwExtension, GlfwExtension + GlfwExtensionCount);

                break;
            }
        }

        m_Instance.Init({.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0)}, InstanceExtension, InstanceLayer);
    }

    VT_CORE_TRACE("Vulkan Instance created");
    /* ===============================================
     *          Variables
     * ===============================================
     */
    std::vector<const char*> DeviceExtension {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef VT_ENABLE_DEBUG
        "VK_EXT_debug_marker"
#endif
    };

    vk::Instance VulkanInstance = m_Instance.Get();
    /* ===============================================
     *          Create Physical device
     * ===============================================
     */
    {
        const auto PDs = VulkanInstance.enumeratePhysicalDevices();
        std::vector<vk::PhysicalDeviceProperties> PDProperties;
        PDProperties.reserve(PDs.size());

        for (const auto& PD : PDs)
        {
            PDProperties.emplace_back(PD.getProperties());
        }

        VT_CORE_ASSERT(
            m_PhysicalDevice.FindPhysicalDevice(PDs, PDProperties, DeviceExtension),
            "Cannot find compatible physical device");
    }

    VT_CORE_TRACE("Vulkan Physical Device created");
    /* ===============================================
     *          Create Window Surface
     * ===============================================
     */
    switch (m_Window->GetWindowAPI())
    {
        case WindowAPI::eGLFWwindow:
        {
            VK_CHECK(
                glfwCreateWindowSurface(
                    VulkanInstance,
                    static_cast<GLFWwindow*>(m_Window->GetNativeWindow()),
                    nullptr,
                    reinterpret_cast<VkSurfaceKHR*>(&m_Surface)),
                vk::Result::eSuccess,
                "Failed to create window surface");
            break;
        }
    }

    VT_CORE_TRACE("Vulkan Window Surface created");
    /* ===============================================
     *          Create Logical device
     * ===============================================
     */
    {
        // add queue
        // m_PhysicalDevice.addQueue(vk::QueueFlagBits::eGraphics, 1.f);
        // m_PhysicalDevice.addQueue(vk::QueueFlagBits::eTransfer, 1.f);
        std::vector<float> f {0.f};
        VT_CORE_ASSERT(
            m_PhysicalDevice.FindGraphicsQueueWithPresent(m_Surface, f),
            "Failed to find graphics queue that can present");

        if (m_PhysicalDevice.SupportsPortabilitySubset())
        {
            DeviceExtension.emplace_back("VK_KHR_portability_subset");
        }

        // create device;
        m_LogicalDevice = m_PhysicalDevice.CreateLogicalDevice(DeviceExtension);
    }

    VT_CORE_TRACE("Vulkan Logical Device created");

    /* ===============================================
     *          Create Swapchain
     * ===============================================
     */
    {
        m_Swapchain.Init(m_Instance.Get(), m_PhysicalDevice.Get(), m_LogicalDevice);

        vk::SwapchainCreateInfoKHR SwapchainInfo {.imageUsage = vk::ImageUsageFlagBits::eColorAttachment};

        {
            const auto [Result, Surfaceformat] = m_Swapchain.FindSurfaceFormat(
                std::vector<vk::SurfaceFormatKHR> {
                    {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}
            },
                m_PhysicalDevice.Get().getSurfaceFormatsKHR(m_Surface));

            VT_CORE_ASSERT(Result, "Could not find required surface format");
            SwapchainInfo.imageFormat     = Surfaceformat.format;
            SwapchainInfo.imageColorSpace = Surfaceformat.colorSpace;
        }

        {
            const auto [Result, PresentMode] = m_Swapchain.FindPresentMode(
                {vk::PresentModeKHR::eFifo},
                m_PhysicalDevice.Get().getSurfacePresentModesKHR(m_Surface));

            VT_CORE_ASSERT(Result, "Could not find required present mode");
            SwapchainInfo.presentMode = PresentMode;
        }

        Swapchain::Capabilities SwapchainQueries {
            .minImageCount    = m_MaxFrameCount,
            .imageExtent      = {m_Window->GetWidth(), m_Window->GetHeight()},
            .arrayLayers      = 1,
            .surfaceTransform = {vk::SurfaceTransformFlagBitsKHR::eIdentity},
            .compositeAlpha   = {vk::CompositeAlphaFlagBitsKHR::eOpaque},
        };

        auto [Result, SwapchainCreateInfo] = m_Swapchain.QueryCapabilities(SwapchainInfo, SwapchainQueries, m_Surface);

        VT_CORE_ASSERT(Result, "Failed to find appropriate swapchain settings");

        if (!m_PhysicalDevice.GraphicsQueueCanPresent())
        {
            uint32_t QueueFamilyIndices[] {
                m_PhysicalDevice.GetGraphicsQueue().queueFamilyIndex,
                m_PhysicalDevice.GetPresentQueue().queueFamilyIndex};

            SwapchainCreateInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
            SwapchainCreateInfo.queueFamilyIndexCount = 2;
            SwapchainCreateInfo.pQueueFamilyIndices   = QueueFamilyIndices;
        }

        m_Swapchain.CreateSwapchain(SwapchainCreateInfo, m_LogicalDevice);
        m_MaxFrameCount = SwapchainCreateInfo.minImageCount;
    }
    VT_CORE_TRACE("Vulkan swapchain created");
    /* ===============================================
     *          Get Graphics and present queues
     * ===============================================
     */
    {
        uint32_t GraphicsQueueFamilyIndex, GraphicsQueueIndex, PresentQueueFamilyIndex, PresentQueueIndex;

        if (m_PhysicalDevice.GraphicsQueueCanPresent())
        {
            const auto DeviceQueueInfo = m_PhysicalDevice.GetGraphicsQueue();
            GraphicsQueueFamilyIndex   = DeviceQueueInfo.queueFamilyIndex;
            // only created 1 queue, so index 0
            GraphicsQueueIndex         = 0;

            PresentQueueFamilyIndex = GraphicsQueueFamilyIndex;
            PresentQueueIndex       = GraphicsQueueIndex;
        }
        else
        {
            auto DeviceQueueInfo     = m_PhysicalDevice.GetGraphicsQueue();
            GraphicsQueueFamilyIndex = DeviceQueueInfo.queueFamilyIndex;
            // use the first queue
            GraphicsQueueIndex       = 0;

            DeviceQueueInfo         = m_PhysicalDevice.GetPresentQueue();
            PresentQueueFamilyIndex = DeviceQueueInfo.queueFamilyIndex;
            // use the first queue
            PresentQueueIndex       = 0;
        }

        m_GraphicQ = m_LogicalDevice.getQueue(GraphicsQueueFamilyIndex, GraphicsQueueIndex);
        m_PresentQ = m_LogicalDevice.getQueue(PresentQueueFamilyIndex, PresentQueueIndex);
    }
    VT_CORE_TRACE("Vulkan graphics and present queues obtained ");
    /* ===============================================
     *          Create Command Pool
     * ===============================================
     */
    {
        m_CmdPool = m_LogicalDevice.createCommandPool(
            {.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
             .queueFamilyIndex = m_PhysicalDevice.GetGraphicsQueue().queueFamilyIndex});
    }

    VT_CORE_TRACE("Vulkan Command pool created");

    /* ===============================================
     *          Create command buffer
     * ===============================================
     */
    {
        vk::CommandBufferAllocateInfo CommandBufferInfo {
            .commandPool        = m_CmdPool,
            .level              = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = m_MaxFrameCount};

        m_DrawBuffer = m_LogicalDevice.allocateCommandBuffers(CommandBufferInfo);
    }
    VT_CORE_TRACE("Vulkan draw buffer (cmd buffer) created");
    /* ===============================================
     *          Synchronization Objs
     * ===============================================
     */
    {
        m_ImageAvailable = m_LogicalDevice.createSemaphore({});
        m_RenderFinished = m_LogicalDevice.createSemaphore({});
        m_DrawFence      = m_LogicalDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
    }
    VT_CORE_TRACE("Vulkan Synchronization objs created");
}

void Context::SwapBuffers() {}

Context::~Context()
{
    m_LogicalDevice.waitIdle();

    m_LogicalDevice.destroySemaphore(m_ImageAvailable);
    m_LogicalDevice.destroySemaphore(m_RenderFinished);
    m_LogicalDevice.destroyFence(m_DrawFence);

    m_LogicalDevice.freeCommandBuffers(m_CmdPool, m_DrawBuffer);
    m_LogicalDevice.destroyCommandPool(m_CmdPool);

    m_LogicalDevice.destroy();
    m_Instance.Get().destroySurfaceKHR(m_Surface);
}

} // namespace VT::Vulkan
