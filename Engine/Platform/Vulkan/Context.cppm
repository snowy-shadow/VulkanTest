module;
#include <vulkan/vulkan.hpp>
#include "EngineMacro.h"

export module VT.Platform.Vulkan.Context;

import VT.Platform.Vulkan.Native.Instance;
import VT.Platform.Vulkan.Native.PhysicalDevice;
import VT.Platform.Vulkan.Native.RenderPass;

import VT.Platform.Vulkan.Device;
import VT.Platform.Vulkan.Swapchain;
import VT.Platform.Vulkan.Synchronization;
import VT.Platform.Vulkan.Attachment;

import VT.RendererContext;
import VT.Util;
import VT.Window;
import VT.Event;

export namespace VT::Vulkan
{
class Context final : public RendererContext
{
public:
    Context(Shared<Window> Window);
    ~Context() override;

public:
    virtual bool BeginFrame() override;
    virtual bool EndFrame() override;

    virtual void OnEvent(Event& Event) override;

    virtual void Init() override;

private:
    void Resize(uint32_t Width, uint32_t Height);

    void CreateResources();
    void DestroyResources();

private:
    Shared<Window> m_Window;

    // Device
    Native::Instance m_Instance;
    Native::PhysicalDevice m_PhysicalDevice;
    LogicalDevice m_LogicalDevice;
    Surface m_Surface;

    // Render
    Swapchain m_Swapchain;
    bool m_ScheduleResize = false;
    Native::RenderPass m_RenderPass;

    // Command Resources
    vk::CommandPool m_CmdPool;
    vk::Queue m_GraphicQ;
    vk::Queue m_PresentQ;
    std::vector<vk::CommandBuffer> m_DrawBuffer;

    // Images
    std::vector<FrameBuffer> m_FrameBuffer;

    // Synchronization
    vk::Semaphore m_ImageAvailable, m_RenderFinished;
    Fence m_DrawFence;

private:
    uint32_t m_MaxFrameCount {};
    uint32_t m_CurrentFrameCount {0};
};
} // namespace VT::Vulkan