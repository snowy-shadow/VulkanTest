module;
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include "EngineMacro.h"

export module VT.Platform.Vulkan.Context;

import VT.Platform.Vulkan.Instance;
import VT.Platform.Vulkan.PhysicalDevice;
import VT.Platform.Vulkan.Swapchain;
import VT.Platform.Vulkan.Attachments;

import VT.RendererContext;
import VT.Util;
import VT.Window;

export namespace VT::Vulkan
{
class Context final : public RendererContext
{
public:
    Context(Shared<Window> Window);
    ~Context();

public:
    void Init() override;
    void SwapBuffers() override;

private:
    Shared<Window> m_Window;

    // TODO : create logical device class, wrap all with Shared
    // Device
    Instance m_Instance;
    PhysicalDevice m_PhysicalDevice;
    vk::Device m_LogicalDevice;
    vk::SurfaceKHR m_Surface;

    // Render
    Swapchain m_Swapchain;
    vk::CommandPool m_CmdPool;
    vk::Queue m_GraphicQ;
    vk::Queue m_PresentQ;
    std::vector<vk::CommandBuffer> m_DrawBuffer;

    DepthStencil m_DepthStencil;

    // Synchronization
    vk::Semaphore m_ImageAvailable, m_RenderFinished;
    vk::Fence m_DrawFence;

private:
    // Util
    uint32_t m_MaxFrameCount;
    uint32_t m_CurrentFrameCount {0};
};
} // namespace VT::Vulkan
