module;
#include "Vulkan.h"

module VT.Platform.Vulkan.Shader;
import VT.Log;

namespace VT::Vulkan
{
void Shader::Create(
    std::span<const HLSL::ShaderFileInfo> Shaders,
    BufferLayout BufferLayout,
    vk::RenderPass Renderpass,
    vk::Device LogicalDevice)
{
    std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfo(Shaders.size());

    HLSL::Compiler ShaderCompiler;

    for (std::size_t i = 0; i < Shaders.size(); i++)
    {
        auto Spv = ShaderCompiler.CompileSpv(Shaders[i]);

        const auto [Result, Module] = LogicalDevice.createShaderModule(
            {.codeSize = Spv.size(), .pCode = reinterpret_cast<uint32_t*>(Spv.data())});
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create shader module");

        ShaderStageInfo[i].stage  = Shaders[i].Stage;
        ShaderStageInfo[i].module = Module;
        ShaderStageInfo[i].pName  = "main";
    }

    std::array<vk::VertexInputBindingDescription, 1> VertexInputBindings {
        {{// binding 0
          .binding = 0,
          // pos, norm, tex coord
          .stride    = BufferLayout.GetStride(),
          .inputRate = vk::VertexInputRate::eVertex}}};


    std::array<vk::VertexInputAttributeDescription, 2> VertexAttributes {
        {
         // pos, 2 floats
         {
         .binding  = VertexInputBindings[0].binding,
         .format   = vk::Format::eR32G32Sfloat,
         },

         // color, 3 floats
    {
         .binding  = VertexInputBindings[0].binding,
    .format  = vk::Format::eR32G32B32Sfloat,
    },
         //// tex coord, 2 floats
         //{
         //	.location = 2,
         //	.binding = VertexInputBindings[0].binding,
         //	.format = vk::Format::eR32G32Sfloat,
         //	.offset = sizeof(float) * (3 + 3)
         //},
        }
    };
    const auto VertexElements = BufferLayout.GetElements();
    for (int i = 0; i < VertexElements.size(); i++)
    {
        VertexAttributes[i].location = i;
        VertexAttributes[i].offset   = VertexElements[i].Offset;
    }

    // const auto& SwapchainInfo{ m_DependencyGraph.get<Swapchain>("SwapChain").getSwapchainCreateInfo().imageExtent};

    vk::PipelineVertexInputStateCreateInfo VertexInputStateInfo {
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(VertexInputBindings.size()),
        .pVertexBindingDescriptions      = VertexInputBindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributes.size()),
        .pVertexAttributeDescriptions    = VertexAttributes.data()};

    // Created graphics pipeline
    m_Pipeline.Create(ShaderStageInfo, {}, VertexInputStateInfo, Renderpass, LogicalDevice);

    // clean up Shader stage
    for (const auto& ShaderStage : ShaderStageInfo)
    {
        LogicalDevice.destroyShaderModule(ShaderStage.module);
    }
}
void Shader::Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint)
{
    m_Pipeline.Bind(CommandBuffer, BindPoint);
}
} // namespace VT::Vulkan
