module;
#include "Vulkan.h"

export module VT.Platform.Vulkan.Shader;

import VT.Platform.Vulkan.Pipeline;
import VT.ShaderCompiler;
import VT.Buffer;
import VT.Camera;
import VT.Platform.Vulkan.Buffer;

export namespace VT::Vulkan
{
class Shader
{
public:
    void Create(
        std::span<const HLSL::ShaderFileInfo> Shaders,
        uint32_t MaxDescriptorSets,
        BufferLayout UniformBufferLayout,
        BufferLayout VertexBufferLayout,
        vk::RenderPass Renderpass,
        vk::PhysicalDevice PhysicalDevice,
        vk::Device LogicalDevice);

    void Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint);

    void UploadUniform(CameraTransform Transform);

    void Destroy();

public:
    Shader()                         = default;
    Shader& operator=(Shader& Other) = delete;
    Shader& operator=(Shader&& Other) noexcept = delete;
    Shader(Shader&& Other) noexcept            = delete;
    Shader(Shader& Other) = delete;
    ~Shader();

private:
    Pipeline m_Pipeline;
    Buffer m_UBO_Buffer;

    vk::DescriptorPool m_DescriptorPool;
    vk::DescriptorSetLayout m_DescriptorLayout;
    std::vector<vk::DescriptorSet> m_DescriptorSet;
    uint32_t m_CurrentDescriptorSet {0};
    uint32_t m_MaxDescriptorSets;

    vk::Device m_LogicalDevice;
};

} // namespace VT::Vulkan
