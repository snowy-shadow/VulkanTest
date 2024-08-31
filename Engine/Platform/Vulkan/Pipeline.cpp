module;
#include "Vulkan.h"

module VT.Platform.Vulkan.Pipeline;

import VT.Log;
import VT.ShaderCompiler;

namespace VT::Vulkan
{
void Pipeline::Create(const std::vector<vk::PipelineShaderStageCreateInfo>& Shaders,
                      vk::PipelineLayoutCreateInfo PipelineLayout,
                      vk::PipelineVertexInputStateCreateInfo VertexInput,
                      vk::RenderPass Renderpass,
                      vk::Device LogicalDevice)
{
    m_LogicalDevice = LogicalDevice;

    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo {
        .topology               = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False,
    };

    vk::PipelineTessellationStateCreateInfo TessellationStateInfo {};

    vk::PipelineViewportStateCreateInfo ViewportStateInfo {
        /*.viewportCount = static_cast<uint32_t>(Viewports.size()),
        .pViewports = Viewports.data(),
        .scissorCount = static_cast<uint32_t>(Scissors.size()),
        .pScissors = Scissors.data()*/

        // using dynamic states
        .viewportCount = 1,
        .scissorCount  = 1,
    };

    std::array<vk::DynamicState, 2> DynamicStates {
        {vk::DynamicState::eViewport, vk::DynamicState::eScissor}
    };

    vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo {
        .dynamicStateCount = static_cast<uint32_t>(DynamicStates.size()), .pDynamicStates = DynamicStates.data()};

    vk::PipelineRasterizationStateCreateInfo RasterizationStateInfo {.depthClampEnable        = vk::False,
                                                                     .rasterizerDiscardEnable = vk::False,
                                                                     .polygonMode             = vk::PolygonMode::eFill,
                                                                     .cullMode        = vk::CullModeFlagBits::eBack,
                                                                     .frontFace       = vk::FrontFace::eClockwise,
                                                                     .depthBiasEnable = vk::False,
                                                                     .lineWidth       = 1.f};

    // Turned Off
    vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo {.rasterizationSamples = vk::SampleCountFlagBits::e1,
                                                                 .sampleShadingEnable  = vk::False};

    std::array<vk::PipelineColorBlendAttachmentState, 1> ColorBlendAttachmentState {
        {{.blendEnable         = vk::False,
          .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
          .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
          .colorBlendOp        = vk::BlendOp::eAdd,
          .srcAlphaBlendFactor = vk::BlendFactor::eOne,
          .dstAlphaBlendFactor = vk::BlendFactor::eZero,
          .alphaBlendOp        = vk::BlendOp::eAdd,
          .colorWriteMask      = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}}};

    vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo {
        .logicOpEnable   = vk::False,
        .attachmentCount = static_cast<uint32_t>(ColorBlendAttachmentState.size()),
        .pAttachments    = ColorBlendAttachmentState.data()};

    vk::PipelineDepthStencilStateCreateInfo DepthStencilStateInfo {.depthTestEnable       = vk::True,
                                                                   .depthWriteEnable      = vk::True,
                                                                   .depthCompareOp        = vk::CompareOp::eLess,
                                                                   .depthBoundsTestEnable = vk::False,
                                                                   .stencilTestEnable     = vk::False};

    // vk::PipelineLayout PipelineLayout{};

    /* =====================================
     *         Pipeline Layout
     * =====================================
     */
    {
        vk::Result Result;
        std::tie(Result, m_Layout) = LogicalDevice.createPipelineLayout(PipelineLayout);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create pipeline layout");
    }

    /* =====================================
     *         Graphics pipeline
     * =====================================
     */
    vk::GraphicsPipelineCreateInfo GraphicPipelineInfo {.stageCount          = static_cast<uint32_t>(Shaders.size()),
                                                        .pStages             = Shaders.data(),
                                                        .pVertexInputState   = &VertexInput,
                                                        .pInputAssemblyState = &InputAssemblyInfo,
                                                        .pTessellationState  = &TessellationStateInfo,
                                                        .pViewportState      = &ViewportStateInfo,
                                                        .pRasterizationState = &RasterizationStateInfo,
                                                        .pMultisampleState   = &MultisampleStateInfo,
                                                        .pDepthStencilState  = &DepthStencilStateInfo,
                                                        .pColorBlendState    = &ColorBlendStateInfo,
                                                        .pDynamicState       = &DynamicStateCreateInfo,
                                                        .layout              = m_Layout,
                                                        .renderPass          = Renderpass,
                                                        .subpass             = 0};

    vk::Result Result;
    std::tie(Result, m_Pipeline) = m_LogicalDevice.createGraphicsPipeline(nullptr, GraphicPipelineInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create graphics pipeline");
}

vk::CommandBuffer Pipeline::Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint)
{
    CommandBuffer.bindPipeline(BindPoint, m_Pipeline);
    return CommandBuffer;
}

void Pipeline::Destroy()
{
    m_LogicalDevice.destroyPipeline(m_Pipeline);
    m_Pipeline = VK_NULL_HANDLE;
    m_LogicalDevice.destroyPipelineLayout(m_Layout);
    m_Layout = VK_NULL_HANDLE;
}

Pipeline::~Pipeline() { Destroy(); }

void GraphicsPipeline::Create(GraphicsPipelineCreateInfo CreateInfo)
{
    uint32_t VertexInputBindingCount;
    auto VertexInputBinding = new vk::VertexInputBindingDescription[VertexInputBindingCount];

    uint32_t VertexInputAttributeCount;
    auto VertexAttribute = new vk::VertexInputAttributeDescription[VertexInputAttributeCount];

    vk::PipelineVertexInputStateCreateInfo VertexInput {.vertexBindingDescriptionCount   = VertexInputBindingCount,
                                                        .pVertexBindingDescriptions      = VertexInputBinding,
                                                        .vertexAttributeDescriptionCount = VertexInputAttributeCount,
                                                        .pVertexAttributeDescriptions    = VertexAttribute};

    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo {.topology = ToVulkan(CreateInfo.PrimitiveInfo.Topology),
                                                                .primitiveRestartEnable =
                                                                    CreateInfo.PrimitiveInfo.PrimitiveRestart};

    vk::PipelineTessellationStateCreateInfo TessellationStateInfo {.patchControlPoints =
                                                                       CreateInfo.Tessellation.has_or({}).ControlPoint};

    vk::PipelineViewportStateCreateInfo ViewPortStateInfo {.viewportCount = CreateInfo.ViewportCount,
                                                           .scissorCount  = CreateInfo.ScissorCount};

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

    const auto& MultiSampleInfo = CreateInfo.MultiSample;
    vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo {
        .rasterizationSamples  = MultiSampleInfo.RasterizationSampleCount,
        .sampleShadingEnable   = MultiSampleInfo.SampleShading,
        .minSampleShading      = MultiSampleInfo.MinSampleShading,
        .alphaToCoverageEnable = MultiSampleInfo.AlphaToCoverage,
        .alphaToOneEnable      = MultiSampleInfo.AlphaToOne};

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
        .maxDepthBounds        = DepthStencilInfo.MaxDepthBounds
    };

    const auto& ColorBlendInfo = CreateInfo.ColorBlend;
    vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo {.logicOpEnable = ColorBlendInfo.LogicOpEnable,
                                                               .logicOp = static_cast<vk::LogicOp>(ColorBlendInfo.LogicOp),
                                                               .attachmentCount = ColorBlendInfo.AttachmentCount,
    };
    vk::PipelineDynamicStateCreateInfo DynamicStateCreateInfo {};
    vk::PipelineLayout PipelineLayoutInfo {};

    vk::RenderPassCreateInfo RenderPassInfo {};

    std::vector<vk::SubpassDependency> SubpassDependency;

    // -1 is null index
    int32_t ParentPipelineIndex = -1;

    Info = {.stageCount          = static_cast<uint32_t>(Shaders.size()),
            .pStages             = Shaders.data(),
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
            .subpass             = static_cast<uint32_t>(SubpassDependency.size()),
            .basePipelineHandle  = Pipeline,
            .basePipelineIndex   = ParentPipelineIndex};

    delete[] VertexInputBinding;
    delete[] VertexAttribute;
}
} // namespace VT::Vulkan
