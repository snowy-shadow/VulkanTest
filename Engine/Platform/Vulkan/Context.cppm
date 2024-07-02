module;
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include "EngineMacro.h"

export module VT.Platform.Vulkan.Context;

import VT.Platform.Vulkan.Native.Instance;
import VT.Platform.Vulkan.Native.PhysicalDevice;
import VT.Platform.Vulkan.Native.RenderPass;

import VT.Platform.Vulkan.Device;
import VT.Platform.Vulkan.Swapchain;

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

private:
    Shared<Window> m_Window;

    // Device
    Native::Instance m_Instance;
    Native::PhysicalDevice m_PhysicalDevice;
    LogicalDevice m_LogicalDevice;
    vk::SurfaceKHR m_Surface;

    // Render
    Swapchain m_Swapchain;
    Native::RenderPass m_RenderPass;

    // Synchronization
    vk::Semaphore m_ImageAvailable, m_RenderFinished;
    vk::Fence m_DrawFence;

private:
    uint32_t m_MaxFrameCount {};
    uint32_t m_CurrentFrameCount {0};

};
} // namespace VT::Vulkan
