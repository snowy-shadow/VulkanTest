module;
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include "EngineMacro.h"

module VT.Platform.Vulkan.Context;

import VT.Log;

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
    };

    vk::Instance VulkanInstance = m_Instance.Get();
    /* ===============================================
     *          Create Physical device
     * ===============================================
     */
    {
        const auto [Result , PDs] = VulkanInstance.enumeratePhysicalDevices();
        VK_CHECK(Result, vk::Result::eSuccess, "Could not enumerate physical devices");

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
        m_LogicalDevice.Init(m_PhysicalDevice.CreateLogicalDevice(DeviceExtension));
    }

    VT_CORE_TRACE("Vulkan Logical Device created");
    /* ===============================================
     *          Variables
     * ===============================================
     */
    vk::Device LogicalDevice = m_LogicalDevice.Get();
   

    /* ===============================================
     *          Synchronization Obj
     * ===============================================
     */
    {
        vk::Result Result;
        std::tie(Result, m_ImageAvailable) = LogicalDevice.createSemaphore({});
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create image available semaphore");

        std::tie(Result, m_RenderFinished) = LogicalDevice.createSemaphore({});
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create render finish semaphore");

        std::tie(Result, m_DrawFence)      = LogicalDevice.createFence({.flags = vk::FenceCreateFlagBits::eSignaled});
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create Draw fence");
    }
    VT_CORE_TRACE("Vulkan Synchronization objs created");

    /* ===============================================
     *          Swapchain
     * ===============================================
     */
    {
        m_Swapchain.Init(*m_Window, LogicalDevice, m_PhysicalDevice, m_Surface, 2);
        m_MaxFrameCount = m_Swapchain.GetMaxFrameCount();
    }

    /* ===============================================
     *          RenderPass
     * ===============================================
     */

    {
        m_RenderPass.Init(LogicalDevice);

        std::vector<vk::AttachmentDescription> Attachment {
            {{.format = m_Swapchain.GetInfo().imageFormat,
              .samples        = vk::SampleCountFlagBits::e1,
              .loadOp         = vk::AttachmentLoadOp::eClear,
              .storeOp        = vk::AttachmentStoreOp::eStore,
              .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
              .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
              .initialLayout  = vk::ImageLayout::eUndefined,
              .finalLayout    = vk::ImageLayout::ePresentSrcKHR}

            }};

        std::vector<vk::AttachmentReference> ColorAttachmentReference {
            {{.attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal}}};

        std::vector<vk::SubpassDescription> Subpass {{{
            .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount = static_cast<uint32_t>(ColorAttachmentReference.size()),
            .pColorAttachments    = ColorAttachmentReference.data(),
        }}};

        std::vector<vk::SubpassDependency> SubpassDependency {
            {{.srcSubpass    = vk::SubpassExternal,
              .dstSubpass    = 0,
              .srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
              .dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
              .srcAccessMask = vk::AccessFlagBits::eNone,
              .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite}}};

        m_RenderPass.Create(Attachment, Subpass, SubpassDependency);
    }

    VT_CORE_TRACE("Renderpass created");
}

Context::~Context()
{
    vk::Device LogicalDevice = m_LogicalDevice.Get();

    LogicalDevice.waitIdle();

    LogicalDevice.destroySemaphore(m_ImageAvailable);
    LogicalDevice.destroySemaphore(m_RenderFinished);
    LogicalDevice.destroyFence(m_DrawFence);

    m_Instance.Get().destroySurfaceKHR(m_Surface);
}

/* ==================================================================
 *                      Private
 * ==================================================================
 */
} // namespace VT::Vulkan
