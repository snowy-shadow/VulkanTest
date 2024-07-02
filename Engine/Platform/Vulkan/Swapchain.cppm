module;
#include <vulkan/vulkan.hpp>

export module VT.Platform.Vulkan.Swapchain;

import VT.Platform.Vulkan.Native.Instance;
import VT.Platform.Vulkan.Native.PhysicalDevice;
import VT.Platform.Vulkan.Native.Swapchain;
import VT.Platform.Vulkan.Attachment;
import VT.Window;

export namespace VT::Vulkan
{
class Swapchain
{
public:
    void Init(Window& Window, vk::Device LogicalDevice, Native::PhysicalDevice& PhysicalDevice, vk::SurfaceKHR Surface, uint32_t MaxFrameCount);
    uint32_t AcquireNextImage(vk::Semaphore Semaphore);

    void Resize(uint32_t Width, uint32_t Height);

    vk::SwapchainKHR Get() const;
    vk::SwapchainCreateInfoKHR GetInfo() const;

    uint32_t GetCurrentFrameCount() const;
    uint32_t GetMaxFrameCount() const;

    ~Swapchain();


private:
    Native::Swapchain m_Swapchain;

    vk::CommandPool m_CmdPool;
    vk::Queue m_GraphicQ;
    vk::Queue m_PresentQ;
    std::vector<vk::CommandBuffer> m_DrawBuffer;

    DepthStencil m_DepthStencil;

    Window* m_Window;
    Native::PhysicalDevice* m_PhysicalDevice;
    vk::Device m_LogicalDevice;
    vk::SurfaceKHR m_Surface;
};
} // namespace VT::Vulkan
