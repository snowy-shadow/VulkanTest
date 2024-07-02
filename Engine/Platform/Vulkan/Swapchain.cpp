module;
#include <vulkan/vulkan.hpp>
#include <limits>
#include "EngineMacro.h"
module VT.Platform.Vulkan.Swapchain;

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
void Swapchain::Init(Window& Window, vk::Device LogicalDevice, Native::PhysicalDevice& PhysicalDevice, vk::SurfaceKHR Surface, uint32_t MaxFrameCount)
{
    m_PhysicalDevice = &PhysicalDevice;
    m_LogicalDevice  = LogicalDevice;
    m_Surface = Surface;
    m_Window = &Window;

    /* ===============================================
     *          Create Swapchain
     * ===============================================
     */
    {
        m_Swapchain.Init(m_PhysicalDevice->Get(), m_LogicalDevice);

        vk::SwapchainCreateInfoKHR SwapchainInfo {.imageUsage = vk::ImageUsageFlagBits::eColorAttachment};

        {
            const auto [R, SF] = m_PhysicalDevice->Get().getSurfaceFormatsKHR(m_Surface);
            
            VK_CHECK(R, vk::Result::eSuccess, "Failed to query surface format");

            const auto [Result, Surfaceformat] = m_Swapchain.FindSurfaceFormat(
                {{{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}}}, SF
                );

            VT_CORE_ASSERT(Result, "Could not find required surface format");
            SwapchainInfo.imageFormat     = Surfaceformat.format;
            SwapchainInfo.imageColorSpace = Surfaceformat.colorSpace;
        }

        {
            const auto [R, PM] = m_PhysicalDevice->Get().getSurfacePresentModesKHR(m_Surface);
            VK_CHECK(R, vk::Result::eSuccess, "Failed to query present mode");

            const auto [Result, PresentMode] = m_Swapchain.FindPresentMode({vk::PresentModeKHR::eFifo}, PM
               );

            VT_CORE_ASSERT(Result, "Could not find required present mode");
            SwapchainInfo.presentMode = PresentMode;
        }

        Native::Swapchain::Capabilities SwapchainQueries {
            .minImageCount    = MaxFrameCount,
            .imageExtent      = {m_Window->GetWidth(), m_Window->GetHeight()},
            .arrayLayers      = 1,
            .surfaceTransform = {vk::SurfaceTransformFlagBitsKHR::eIdentity},
            .compositeAlpha   = {vk::CompositeAlphaFlagBitsKHR::eOpaque},
        };

        auto [Result, SwapchainCreateInfo] = m_Swapchain.QueryCapabilities(SwapchainInfo, SwapchainQueries, m_Surface);

        VT_CORE_ASSERT(Result, "Failed to find appropriate swapchain settings");

        if (!m_PhysicalDevice->GraphicsQueueCanPresent())
        {
            uint32_t QueueFamilyIndices[] {
                m_PhysicalDevice->GetGraphicsQueue().queueFamilyIndex,
                m_PhysicalDevice->GetPresentQueue().queueFamilyIndex};

            SwapchainCreateInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
            SwapchainCreateInfo.queueFamilyIndexCount = 2;
            SwapchainCreateInfo.pQueueFamilyIndices   = QueueFamilyIndices;
        }

        m_Swapchain.CreateSwapchain(SwapchainCreateInfo, m_LogicalDevice);
    }
    VT_CORE_TRACE("Vulkan swapchain created");

    /* ===============================================
     *          Depth Image
     * ===============================================
     */
    {
        auto [Result, DepthFormat] = m_PhysicalDevice->FindSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        VT_CORE_ASSERT(Result, "Failed to find depth format");

        m_DepthStencil.Create(
            m_Swapchain.GetInfo().imageExtent,
            vk::SampleCountFlagBits::e1,
            DepthFormat,
            m_LogicalDevice,
            *m_PhysicalDevice);
    }
    VT_CORE_TRACE("Depth resource created");

     /* ===============================================
     *          Get Graphics and present queues
     * ===============================================
     */
    {
        uint32_t GraphicsQueueFamilyIndex, GraphicsQueueIndex, PresentQueueFamilyIndex, PresentQueueIndex;

        if (m_PhysicalDevice->GraphicsQueueCanPresent())
        {
            const auto DeviceQueueInfo = m_PhysicalDevice->GetGraphicsQueue();
            GraphicsQueueFamilyIndex   = DeviceQueueInfo.queueFamilyIndex;
            // only created 1 queue, so index 0
            GraphicsQueueIndex         = 0;

            PresentQueueFamilyIndex = GraphicsQueueFamilyIndex;
            PresentQueueIndex       = GraphicsQueueIndex;
        }
        else
        {
            auto DeviceQueueInfo     = m_PhysicalDevice->GetGraphicsQueue();
            GraphicsQueueFamilyIndex = DeviceQueueInfo.queueFamilyIndex;
            // use the first queue
            GraphicsQueueIndex       = 0;

            DeviceQueueInfo         = m_PhysicalDevice->GetPresentQueue();
            PresentQueueFamilyIndex = DeviceQueueInfo.queueFamilyIndex;
            // use the first queue
            PresentQueueIndex       = 0;
        }

        m_GraphicQ = LogicalDevice.getQueue(GraphicsQueueFamilyIndex, GraphicsQueueIndex);
        m_PresentQ = LogicalDevice.getQueue(PresentQueueFamilyIndex, PresentQueueIndex);
    }
    VT_CORE_TRACE("Vulkan graphics and present queues obtained ");

        /* ===============================================
     *          Create Command Pool
     * ===============================================
     */
    {
        vk::Result Result;
        std::tie(Result, m_CmdPool) = LogicalDevice.createCommandPool(
            {.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
             .queueFamilyIndex = m_PhysicalDevice->GetGraphicsQueue().queueFamilyIndex});

        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create command pool");
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
            .commandBufferCount = m_Swapchain.GetInfo().minImageCount};

        vk::Result Result;
        std::tie(Result, m_DrawBuffer) = LogicalDevice.allocateCommandBuffers(CommandBufferInfo);

        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create command pool");
    }
    VT_CORE_TRACE("Vulkan draw buffer (cmd buffer) created");
}

uint32_t Swapchain::AcquireNextImage(vk::Semaphore Semaphore)
{
    const auto [Result, ImageIndex] = m_LogicalDevice.acquireNextImageKHR(
        m_Swapchain.Get(),
        std::numeric_limits<uint64_t>::max(),
        Semaphore,
        VK_NULL_HANDLE);

    if (Result != vk::Result::eSuccess)
    {
        Resize(m_Window->GetWidth(), m_Window->GetHeight());
        return AcquireNextImage(Semaphore);
    }

    return ImageIndex;
}

vk::SwapchainKHR Swapchain::Get() const { return m_Swapchain.Get();}
vk::SwapchainCreateInfoKHR Swapchain::GetInfo() const { return m_Swapchain.GetInfo(); }

uint32_t Swapchain::GetMaxFrameCount() const { return m_Swapchain.GetInfo().minImageCount; }

void Swapchain::Resize(uint32_t Width, uint32_t Height)
{
    auto Info = m_Swapchain.GetInfo();

    Info.imageExtent = {Width, Height};

    // update surface capabilities, else it will complain
    // cast to void, ignore output - warn unused variable
    (void) m_PhysicalDevice->Get().getSurfaceCapabilitiesKHR(m_Surface);

    m_Swapchain.RecreateSwapchain(Info, m_LogicalDevice);
}

Swapchain::~Swapchain()
{
    m_LogicalDevice.freeCommandBuffers(m_CmdPool, m_DrawBuffer);
    m_LogicalDevice.destroyCommandPool(m_CmdPool);
    m_DepthStencil.Destroy();
    m_LogicalDevice.destroySwapchainKHR(m_Swapchain.Get());
}
} // namespace VT::Vulkan
