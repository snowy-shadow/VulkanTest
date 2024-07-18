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
import VT.Platform.Vulkan.Pipeline;
import VT.Platform.Vulkan.Buffer;
import VT.Platform.Vulkan.Shader;

import VT.RendererType;
import VT.RendererContext;
import VT.Util;
import VT.Window;
import VT.Event;
import VT.Buffer;
import VT.Camera;
import VT.Texture;
import VT.Timestep;

export namespace VT::Vulkan
{
class RendererContext final : public VT::RendererContext
{
public:
    RendererContext(Shared<Window> Window);
    ~RendererContext() override;

public:
    virtual bool BeginFrame() override;
    virtual bool EndFrame() override;

    virtual Uniq<Texture> CreateTexture(const TextureCreateInfo& TextureInfo) override;
    void UploadView(RendererType::UniformCameraData Data);
    void UploadGeometry(RendererType::GeometryRenderData Data);

    virtual void OnUpdate(const Timestep& Time) override;
    virtual void OnEvent(Event& Event) override;

    virtual void Init() override;

private:
    void Resize(uint32_t Width, uint32_t Height);

    void UploadData(vk::Buffer Dest, void* Data, uint32_t Size, uint32_t Offset, vk::Queue, vk::Fence) const;

    void CreateResources();
    void DestroyResources();

private:
    Shared<Window> m_Window;
    Uniq<Camera> m_Camera;

    // Device
    Native::Instance m_Instance;
    Native::PhysicalDevice m_PhysicalDevice;
    LogicalDevice m_LogicalDevice;
    Surface m_Surface;

    // Render
    Swapchain m_Swapchain;
    bool m_ScheduleResize = false;
    Native::RenderPass m_RenderPass;
    Shader m_TriangleShader;

    // Command Resources
    vk::CommandPool m_CmdPool;
    vk::Queue m_GraphicQ;
    vk::Queue m_PresentQ;
    std::vector<vk::CommandBuffer> m_DrawBuffer;


    // Images && buffers
    std::vector<FrameBuffer> m_FrameBuffer;
    VulkanBuffer m_VertexBuffer;
    VulkanBuffer m_IndexBuffer;

    // Synchronization
    Fence m_DrawFence;
    bool bm_UpdateCameraTransform {false};

private:
    uint32_t m_MaxFrameCount {};
    uint32_t m_CurrentFrameCount {0};
    Timestep m_DeltaTime;
};
} // namespace VT::Vulkan