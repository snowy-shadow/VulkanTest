module;
#include <vulkan/vulkan.hpp>

export module VT.Platform.Vulkan.Pipeline;

export namespace VT::Vulkan
{
class Pipeline
{
public:
    void Create(
        const std::vector<vk::PipelineShaderStageCreateInfo>& Shaders,
        vk::PipelineLayoutCreateInfo PipelineLayout,
        vk::PipelineVertexInputStateCreateInfo VertexInput,
        vk::RenderPass Renderpass,
        vk::Device LogicalDevice);
    vk::CommandBuffer Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint);

    void Destroy();
    ~Pipeline();

private:
    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;

    vk::Device m_LogicalDevice;
};
} // namespace VT::Vulkan
