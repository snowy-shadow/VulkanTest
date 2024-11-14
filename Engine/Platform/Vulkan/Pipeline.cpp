module;
#include "Vulkan.h"

#include "EngineMacro.h"

module VT.Platform.Vulkan.Pipeline;

import VT.Log;
import VT.ShaderCompiler;

namespace VT::Vulkan
{
// void Pipeline::Create(const std::vector<vk::PipelineShaderStageCreateInfo>& Shaders,
//                       vk::PipelineLayoutCreateInfo PipelineLayout,
//                       vk::PipelineVertexInputStateCreateInfo VertexInput,
//                       vk::RenderPass Renderpass,
//                       vk::Device LogicalDevice)
// {
//     m_LogicalDevice = LogicalDevice;
//
//     vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo {
//         .topology               = vk::PrimitiveTopology::eTriangleList,
//         .primitiveRestartEnable = vk::False,
//     };
//
//     vk::PipelineTessellationStateCreateInfo TessellationStateInfo {};
//
//     vk::PipelineViewportStateCreateInfo ViewportStateInfo {
//         /*.viewportCount = static_cast<uint32_t>(Viewports.size()),
//         .pViewports = Viewports.data(),
//         .scissorCount = static_cast<uint32_t>(Scissors.size()),
//         .pScissors = Scissors.data()*/
//
//         // using dynamic states
//         .viewportCount = 1,
//         .scissorCount  = 1,
//     };
//
//     std::array<vk::DynamicState, 2> DynamicStates {
//         {vk::DynamicState::eViewport, vk::DynamicState::eScissor}
//     };
//
//     vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo {
//         .dynamicStateCount = static_cast<uint32_t>(DynamicStates.size()), .pDynamicStates = DynamicStates.data()};
//
//     vk::PipelineRasterizationStateCreateInfo RasterizationStateInfo {.depthClampEnable        = vk::False,
//                                                                      .rasterizerDiscardEnable = vk::False,
//                                                                      .polygonMode             =
//                                                                      vk::PolygonMode::eFill, .cullMode        =
//                                                                      vk::CullModeFlagBits::eBack, .frontFace       =
//                                                                      vk::FrontFace::eClockwise, .depthBiasEnable =
//                                                                      vk::False, .lineWidth       = 1.f};
//
//     // Turned Off
//     vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo {.rasterizationSamples = vk::SampleCountFlagBits::e1,
//                                                                  .sampleShadingEnable  = vk::False};
//
//     std::array<vk::PipelineColorBlendAttachmentState, 1> ColorBlendAttachmentState {
//         {{.blendEnable         = vk::False,
//           .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
//           .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
//           .colorBlendOp        = vk::BlendOp::eAdd,
//           .srcAlphaBlendFactor = vk::BlendFactor::eOne,
//           .dstAlphaBlendFactor = vk::BlendFactor::eZero,
//           .alphaBlendOp        = vk::BlendOp::eAdd,
//           .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
//                             vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}}};
//
//     vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo {
//         .logicOpEnable   = vk::False,
//         .attachmentCount = static_cast<uint32_t>(ColorBlendAttachmentState.size()),
//         .pAttachments    = ColorBlendAttachmentState.data()};
//
//     vk::PipelineDepthStencilStateCreateInfo DepthStencilStateInfo {.depthTestEnable       = vk::True,
//                                                                    .depthWriteEnable      = vk::True,
//                                                                    .depthCompareOp        = vk::CompareOp::eLess,
//                                                                    .depthBoundsTestEnable = vk::False,
//                                                                    .stencilTestEnable     = vk::False};
//
//     // vk::PipelineLayout PipelineLayout{};
//
//     /* =====================================
//      *         Pipeline Layout
//      * =====================================
//      */
//     {
//         vk::Result Result;
//         std::tie(Result, m_Layout) = LogicalDevice.createPipelineLayout(PipelineLayout);
//         VK_CHECK(Result, vk::Result::eSuccess, "Failed to create pipeline layout");
//     }
//
//     /* =====================================
//      *         Graphics pipeline
//      * =====================================
//      */
//     vk::GraphicsPipelineCreateInfo GraphicPipelineInfo {.stageCount          = static_cast<uint32_t>(Shaders.size()),
//                                                         .pStages             = Shaders.data(),
//                                                         .pVertexInputState   = &VertexInput,
//                                                         .pInputAssemblyState = &InputAssemblyInfo,
//                                                         .pTessellationState  = &TessellationStateInfo,
//                                                         .pViewportState      = &ViewportStateInfo,
//                                                         .pRasterizationState = &RasterizationStateInfo,
//                                                         .pMultisampleState   = &MultisampleStateInfo,
//                                                         .pDepthStencilState  = &DepthStencilStateInfo,
//                                                         .pColorBlendState    = &ColorBlendStateInfo,
//                                                         .pDynamicState       = &DynamicStateCreateInfo,
//                                                         .layout              = m_Layout,
//                                                         .renderPass          = Renderpass,
//                                                         .subpass             = 0};
//
//     vk::Result Result;
//     std::tie(Result, m_Pipeline) = m_LogicalDevice.createGraphicsPipeline(nullptr, GraphicPipelineInfo);
//     VK_CHECK(Result, vk::Result::eSuccess, "Failed to create graphics pipeline");
// }
//
// vk::CommandBuffer Pipeline::Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint)
// {
//     CommandBuffer.bindPipeline(BindPoint, m_Pipeline);
//     return CommandBuffer;
// }
//
// void Pipeline::Destroy()
// {
//     m_LogicalDevice.destroyPipeline(m_Pipeline);
//     m_Pipeline = VK_NULL_HANDLE;
//     m_LogicalDevice.destroyPipelineLayout(m_Layout);
//     m_Layout = VK_NULL_HANDLE;
// }
//
// Pipeline::~Pipeline() { Destroy(); }

void GraphicsPipeline::Create(GraphicsPipelineCreateInfo CreateInfo)
{
    LogicalDevice            = CreateInfo.LogicalDevice.Get();
    //////////      Vertex Description     ///////////////////////
    const auto& VertexLayout = CreateInfo.VertexLayout;
    std::vector<vk::VertexInputBindingDescription> VertexInputBinding(VertexLayout.size());
    std::vector<vk::VertexInputAttributeDescription> VertexAttribute;

    // FIX : is it vertexlayout[i] or loop through vertexlayout again?
    for (uint32_t i = 0; i < VertexLayout.size(); i++)
    {
        VertexInputBinding[i].binding = i;
        VertexInputBinding[i].stride  = VertexLayout[i].GetStride();

        const auto& VertexAttrib = VertexLayout[i].GetElemnts();
        for (uint32_t j = 0; j < VertexAttrib.size(); j++)
        {
            // location, binding. format, offset
            VertexAttribute.emplace_back(j, i, static_cast<vk::Format>(VertexAttrib[j].Format), VertexAttrib[j].Offset);
        }
    }

    vk::PipelineVertexInputStateCreateInfo VertexInput {
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(VertexInputBinding.size()),
        .pVertexBindingDescriptions      = VertexInputBinding.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttribute.size()),
        .pVertexAttributeDescriptions    = VertexInputBinding.data()};

    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo {
        .topology               = static_cast<vk::PrimitiveTopology>(CreateInfo.PrimitiveInfo.Topology),
        .primitiveRestartEnable = CreateInfo.PrimitiveInfo.PrimitiveRestart};

    //////////      Tessellation     ///////////////////////
    vk::PipelineTessellationStateCreateInfo TessellationStateInfo {.patchControlPoints =
                                                                       CreateInfo.Tessellation.ControlPoint};

    //////////      Viewport / Scissor     ///////////////////////
    vk::PipelineViewportStateCreateInfo ViewPortStateInfo {.viewportCount = CreateInfo.ViewportCount,
                                                           .scissorCount  = CreateInfo.ScissorCount};

    //////////      Raster     ///////////////////////
    const auto& RasterInfo = CreateInfo.Rasterization;
    vk::PipelineRasterizationStateCreateInfo RasterizaitonStateInfo {
        .depthClampEnable        = RasterInfo.DepthClamp,
        .rasterizerDiscardEnable = RasterInfo.RasterizerDiscard,
        .polygonMode             = static_cast<vk::PolygonMode>(RasterInfo.PolygonMode),
        .cullMode                = static_cast<vk::CullModeFlagBits>(RasterInfo.CullMode),
        .frontFace               = static_cast<vk::FrontFace>(FrontFace),
        .depthBiasEnable         = RasterInfo.DepthBias,
        .depthBiasConstantFactor = RasterInfo.DepthBiasConstantFactor,
        .depthBiasClamp          = RasterInfo.DepthBiasClmap,
        .depthBiasSlopeFactor    = RasterInfo.DepthBiasSlopeFactor,
        .lineWidth               = RasterInfo.LineWidth};

    //////////      Multisample     ///////////////////////
    const auto& MultiSampleInfo = CreateInfo.MultiSample;
    vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo {
        .rasterizationSamples  = MultiSampleInfo.RasterizationSampleCount,
        .sampleShadingEnable   = MultiSampleInfo.SampleShading,
        .minSampleShading      = MultiSampleInfo.MinSampleShading,
        .alphaToCoverageEnable = MultiSampleInfo.AlphaToCoverage,
        .alphaToOneEnable      = MultiSampleInfo.AlphaToOne};

    //////////      Depth stencil     ///////////////////////
    const auto& DepthStencilInfo = CreateInfo.DepthStencil;
    vk::PipelineDepthStencilStateCreateInfo DepthStencilStateInfo {
        .depthTestEnable       = DepthStencilInfo.DepthTest,
        .depthWriteEnable      = DepthStencilInfo.DepthWrite,
        .depthCompareOp        = static_cast<vk::CompareOp>(DepthStencilInfo.DepthCompareOp),
        .depthBoundsTestEnable = DepthStencilInfo.DepthBoundsTest,
        .stencilTestEnable     = DepthStencilInfo.StencilTest,
        .front                 = static_cast<vk::StencilOpState>(DepthStencilInfo.Front),
        .back                  = static_cast<vk::StencilOpState>(DepthStencilInfo.Back),
        .minDepthBounds        = DepthStencilInfo.MinDepthBounds,
        .maxDepthBounds        = DepthStencilInfo.MaxDepthBounds};

    //////////      Color Blend     ///////////////////////
    vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo {
        .logicOpEnable   = CreateInfo.ColorBlend.LogicOpEnable,
        .logicOp         = static_cast<vk::LogicOp>(CreateInfo.ColorBlend.LogicOp),
        .attachmentCount = CreateInfo.ColorBlend.AttachmentCount,
    };

    //////////      Dynamic states     ///////////////////////
    const std::array DynamicStates {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo {
        .dynamicStateCount = static_cast<uint32_t>(DynamicStates.size()), .pDynamicStates = DynamicStates.data()};

    //////////      Pipeline Layout &&   Shader stage     ///////////////////////
    std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfo(CreateInfo.Shader.size());
    std::vector<vk::DescriptorSetLayout> DescriptorSetLayout(CreateInfo.Shader.size());
    std::vector<vk::PushConstantRange> PushConstantInfo;
    uint32_t PC_Offset = 0;

    for (const auto& ShaderInfo : CreateInfo.Shader)
    {
        const auto [Result, Module] =
            LogicalDevice.createShaderModule({.codeSize = ShaderInfo.Spv.size(), .pCode = ShaderInfo.Spv.data()});
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create shader module");

        ShaderStageInfo.emplace_back({}, ShaderInfo.Stage, Module, "main");

        // make a copy and store
        // indirection operator `*` returns a lvalue
        DescriptorSetLayout.push_back(*static_cast<const vk::DescriptroSetLayout*>(ShaderInfo.Layout.GetHandle()));

        if (ShaderInfo.Layout.PushConstantSize != 0)
        {
            VT_CORE_ASSERT(ShaderInfo.Layout.PushConstantSize % 4 == 0,
                           "Vulkan Push Constants must be in multiples of 4 bytes");

            PushConstantInfo.emplace_back(static_cast<vk::ShaderStageFlags>(ShaderInfo.Layout.Stage),
                                          PC_Offset,
                                          ShaderInfo.Layout.PushConstantSize);
            PC_Offset += ShaderInfo.Layout.PushConstantSize;
        }
    }
    vk::PipelineLayoutCreateInfo PipelineLayoutInfo {.setLayoutCount = static_cast<uint32_t>(CreateInfo.Shader.size()),
                                                     .pSetLayouts    = DescriptorSetLayout.data(),
                                                     .pushConstantRangeCount =
                                                         static_cast<uint32_t>(PushConstantInfo.size()),
                                                     .pPushConstantRanges = PushConstantInfo.data()};

    vk::Result Result;
    std::tie(Result, Pipeline) = LogicalDevice.createGraphicsPipeline({
        .stageCount          = static_cast<uint32_t>(ShaderStageInfo.size()),
        .pStages             = ShaderStageInfo.data(),
        .pVertexInputState   = &VertexInput,
        .pInputAssemblyState = &InputAssemblyInfo,
        .pTessellationState  = &TessellationStateInfo,
        .pViewportState      = &ViewportStateInfo,
        .pRasterizationState = &RasterizationStateInfo,
        .pMultisampleState   = &MultisampleStateInfo,
        .pDepthStencilState  = &DepthStencilStateInfo,
        .pColorBlendState    = &ColorBlendStateInfo,
        .pDynamicState       = &DynamicStateCreateInfo,
        .layout              = PipelineLayoutInfo,
        .renderPass          = Renderpass,
    });
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create graphics pipeline");

    // clean up Shader stage
    for (const auto& ShaderStage : ShaderStageInfo)
    {
        LogicalDevice.destroyShaderModule(ShaderStage.module);
    }
}
} // namespace VT::Vulkan
