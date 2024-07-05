module;
#include "Vulkan.h"

module VT.Platform.Vulkan.Pipeline;

import VT.Log;
import VT.ShaderCompiler;

namespace VT::Vulkan
{
void Pipeline::Create(
    const std::vector<vk::PipelineShaderStageCreateInfo>& Shaders,
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

    // std::array<vk::Viewport, 1> Viewports
    //{ {
    //	{
    //		.x = 0.f,
    //		.y = 0.f,
    //		.width = static_cast<float>(SwapchainInfo.width),
    //		.height = static_cast<float>(SwapchainInfo.height),
    //		.minDepth = 0.f,
    //		.maxDepth = 1.f
    //	}
    // } };

    // std::array<vk::Rect2D, 1> Scissors
    //{ {

    //	{
    //		.offset =
    //		{
    //			.x = 0,
    //			.y = 0
    //		},
    //		.extent =
    //		{
    //			.width = SwapchainInfo.width,
    //			.height = SwapchainInfo.height,
    //		}
    //	}
    //} };

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
        .dynamicStateCount = static_cast<uint32_t>(DynamicStates.size()),
        .pDynamicStates    = DynamicStates.data()};

    vk::PipelineRasterizationStateCreateInfo RasterizationStateInfo {
        .depthClampEnable        = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode             = vk::PolygonMode::eFill,
        .cullMode                = vk::CullModeFlagBits::eBack,
        .frontFace               = vk::FrontFace::eClockwise,
        .depthBiasEnable         = vk::False,
        .lineWidth               = 1.f};

    // Turned Off
    vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
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

    vk::PipelineDepthStencilStateCreateInfo DepthStencilStateInfo {
        .depthTestEnable       = vk::True,
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
    vk::GraphicsPipelineCreateInfo GraphicPipelineInfo {
        .stageCount          = static_cast<uint32_t>(Shaders.size()),
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
} // namespace VT::Vulkan
