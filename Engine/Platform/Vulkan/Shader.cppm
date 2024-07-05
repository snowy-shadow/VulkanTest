module;
#include "Vulkan.h"

export module VT.Platform.Vulkan.Shader;

import VT.Platform.Vulkan.Pipeline;
import VT.ShaderCompiler;
import VT.Buffer;

export namespace VT::Vulkan
{
class Shader
{
public:
    void Create(
        std::span<const HLSL::ShaderFileInfo> Shaders,
        BufferLayout BufferLayout,
        vk::RenderPass Renderpass,
        vk::Device LogicalDevice);

    void Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint);

    void Destroy() { m_Pipeline.Destroy(); }

    ~Shader() { Destroy(); }
private:
    Pipeline m_Pipeline;
};

} // namespace VT::Vulkan
