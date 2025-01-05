module;
#include "Vulkan.h"
#include "EngineMacro.h"
#include <ranges>
module VT.Platform.Vulkan.PipelineManager;

import VT.Log;
import VT.ShaderCompiler;
import VT.Platform.Vulkan.TypeConversion;

namespace VT::Vulkan
{
//////////      Descriptor Layout     ///////////////////////
bool ResourceManager::CreateDescriptorLayout(const std::vector<VT::DescriptorLayoutBindingInfo>& CreateInfo,
                                             std::string Name)
{
    if (m_DescriptorLayout.contains(Name) || CreateInfo.empty())
    {
        return false;
    }

    std::vector<vk::DescriptorSetLayoutBinding> LayoutBindings;
    for (const auto& Layout : CreateInfo)
    {
        LayoutBindings.push_back(Convert::DescriptorSetLayoutBinding(Layout));
    }
    const auto& [Result, Handle] = m_LogicalDevice.createDescriptorSetLayout(
        {.bindingCount = static_cast<uint32_t>(LayoutBindings.size()), .pBindings = LayoutBindings.data()});

    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor layout handle");

    return m_DescriptorLayout.emplace(Name, Handle).second;
}
bool ResourceManager::RemoveDescriptorLayout(const std::string& Name)
{
    m_LogicalDevice.destroyDescriptorSetLayout(m_DescriptorLayout[Name]);
    return m_DescriptorLayout.erase(Name) == 1;
}
bool ResourceManager::Allocate(const VT::DescriptorAllocateInfo& Info, const char* Name)
{
    if (!m_DescriptorPool.contains(Info.Pool))
    {
        return false;
    }
    std::vector<vk::DescriptorSetLayout> Sets;
    Sets.reserve(Info.DescriptorLayouts.size());

    for (const auto& Layout : Info.DescriptorLayouts)
    {
        if (!m_DescriptorLayout.contains(Layout))
        {
            return false;
        }
        Sets.emplace_back(m_DescriptorLayout[Layout]);
    }

    const auto& [Result, Handle] =
        m_LogicalDevice.allocateDescriptorSets({.descriptorPool     = m_DescriptorPool[Info.Pool],
                                                .descriptorSetCount = static_cast<uint32_t>(Sets.size()),
                                                .pSetLayouts        = Sets.data()});

    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate descriptor sets");
    if (Result != vk::Result::eSuccess)
    {
        return false;
    }

    return m_DescriptorSet.emplace(Name, Handle).second;
}

bool ResourceManager::CreateDescriptorPool(const VT::DescriptorPoolInfo& PoolInfo, const char* Name)
{
    std::vector<vk::DescriptorPoolSize> PoolSizes;
    PoolSizes.reserve(PoolInfo.PoolSize.size());
    for (const auto& PoolSize : PoolInfo.PoolSize)
    {
        PoolSizes.emplace_back(Vulkan::Convert::DescriptorPoolSize(PoolSize));
    }
    const auto& [R, Pool] =
        m_LogicalDevice.createDescriptorPool({.maxSets       = PoolInfo.MaxSets,
                                              .poolSizeCount = static_cast<uint32_t>(PoolSizes.size()),
                                              .pPoolSizes    = PoolSizes.data()});
    VK_CHECK(R, vk::Result::eSuccess, "Failed to create descriptor pool");
    if (R != vk::Result::eSuccess)
    {
        return false;
    }

    return m_DescriptorPool.emplace(Name, Pool).second;
}

//////////      Pipeline Layout     ///////////////////////
bool ResourceManager::CreatePipelineLayout(std::vector<std::string> DescriptorLayouts,
                                           std::vector<PushConstantInfo> PushConstants,
                                           std::string Name)
{
    if (m_PipelineLayout.contains(Name) || (DescriptorLayouts.empty() && PushConstants.empty()))
    {
        return false;
    }

    //////////      Pipeline Layout &&   Shader stage     ///////////////////////
    std::vector<vk::DescriptorSetLayout> DescriptorSetLayout(DescriptorLayouts.size());

    for (const auto& DescriptorId : DescriptorLayouts)
    {
        if (!m_DescriptorLayout.contains(DescriptorId))
        {
            VT_ERROR("Invalid Descriptor ID : {}", DescriptorId);
            return false;
        }
        const auto& DescriptorLayout = m_DescriptorLayout[DescriptorId];
    }

    uint32_t PC_Offset = 0;
    std::vector<vk::PushConstantRange> PushConstantInfo;
    for (const auto& PushConstant : PushConstants)
    {
        const auto PC_Size = PushConstant.Size;
        if (PC_Size != 0)
        {
            VT_CORE_ASSERT(PC_Size % 4 == 0, "Vulkan Push Constants must be in multiples of 4 bytes");

            PushConstantInfo.push_back(
                {.stageFlags = static_cast<vk::ShaderStageFlags>(PushConstant.StageFlags.Value()),
                 .offset     = PC_Offset,
                 .size       = PC_Size});
            PC_Offset += PC_Size;
        }
    }
    const auto& [Result, Handle] =
        m_LogicalDevice.createPipelineLayout({.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayout.size()),
                                              .pSetLayouts    = DescriptorSetLayout.data(),
                                              .pushConstantRangeCount = static_cast<uint32_t>(PushConstantInfo.size()),
                                              .pPushConstantRanges    = PushConstantInfo.data()});

    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create pipeline layout");

    return m_PipelineLayout.emplace(Name, Handle).second;
}
bool ResourceManager::RemovePipelineLayout(const std::string& Name) { m_PipelineLayout.erase(Name); }

ResourceManager::ResourceManager(vk::Device D) : m_LogicalDevice(D) {}

ResourceManager::~ResourceManager()
{
    for (const auto& P : m_PipelineLayout | std::ranges::views::values)
    {
        m_LogicalDevice.destroyPipelineLayout(P);
    }

    for (const auto& DL : m_DescriptorLayout | std::ranges::views::values)
    {
        m_LogicalDevice.destroyDescriptorSetLayout(DL);
    }
}

//////////      Pipeline Manager     ///////////////////////
bool PipelineManager::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& CreateInfo, std::string Name)
{
    if (!m_RenderPass.contains(CreateInfo.RenderPass) ||
        !m_ResourceManager.m_PipelineLayout.contains(CreateInfo.PipelineLayout) || m_Pipeline.contains(Name))
    {
        return false;
    }

    //////////      Shader stage     ///////////////////////
    std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfo;
    ShaderStageInfo.reserve(CreateInfo.Shader.size());

    for (const auto& ShaderInfo : CreateInfo.Shader)
    {
        const auto [Result, Module] =
            m_LogicalDevice.createShaderModule({.codeSize = ShaderInfo.Spv.size(), .pCode = ShaderInfo.Spv.data()});
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create shader module");
        if (static_cast<vk::Result>(Result) != vk::Result::eSuccess)
        {
            goto Failed;
        }

        ShaderStageInfo.push_back(
            {.stage = static_cast<vk::ShaderStageFlagBits>(ShaderInfo.Stage), .module = Module, .pName = "main"});
    }

    //////////      Vertex Description     ///////////////////////
    const auto& VertexLayout = CreateInfo.VertexLayout;
    std::vector<vk::VertexInputBindingDescription> VertexInputBinding(VertexLayout.size());
    std::vector<vk::VertexInputAttributeDescription> VertexAttribute;

    // FIX : is it vertexlayout[i] or loop through vertexlayout again?
    for (uint32_t i = 0; i < VertexLayout.size(); i++)
    {
        VertexInputBinding[i].binding = i;
        VertexInputBinding[i].stride  = VertexLayout[i].GetStride();

        const auto& VertexAttrib = VertexLayout[i].GetElements();
        for (uint32_t j = 0; j < VertexAttrib.size(); j++)
        {
            // location, binding. format, offset
            VertexAttribute.push_back({.location = j,
                                       .binding  = i,
                                       .format   = static_cast<vk::Format>(VertexAttrib[j].DataFormat),
                                       .offset   = VertexAttrib[j].Offset});
        }
    }

    vk::PipelineVertexInputStateCreateInfo VertexInput {
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(VertexInputBinding.size()),
        .pVertexBindingDescriptions      = VertexInputBinding.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttribute.size()),
        .pVertexAttributeDescriptions    = VertexAttribute.data()};

    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo {
        .topology               = static_cast<vk::PrimitiveTopology>(CreateInfo.PrimitiveInfo.Topology),
        .primitiveRestartEnable = CreateInfo.PrimitiveInfo.PrimitiveRestart};

    //////////      Tessellation     ///////////////////////
    vk::PipelineTessellationStateCreateInfo TessellationStateInfo {
        .patchControlPoints = CreateInfo.Tessellation.value_or({}).ControlPoint};

    //////////      Viewport / Scissor     ///////////////////////
    vk::PipelineViewportStateCreateInfo ViewPortStateInfo {.viewportCount = CreateInfo.ViewportCount,
                                                           .scissorCount  = CreateInfo.ScissorCount};

    //////////      Raster     ///////////////////////
    const auto& RasterInfo = CreateInfo.Rasterization;
    vk::PipelineRasterizationStateCreateInfo RasterizationStateInfo {
        .depthClampEnable        = RasterInfo.DepthClamp,
        .rasterizerDiscardEnable = RasterInfo.RasterizerDiscard,
        .polygonMode             = static_cast<vk::PolygonMode>(RasterInfo.PolygonMode),
        .cullMode                = static_cast<vk::CullModeFlagBits>(RasterInfo.CullMode),
        .frontFace               = static_cast<vk::FrontFace>(RasterInfo.FrontFace),
        .depthBiasEnable         = RasterInfo.DepthBias,
        .depthBiasConstantFactor = RasterInfo.DepthBiasConstantFactor,
        .depthBiasClamp          = RasterInfo.DepthBiasClamp,
        .depthBiasSlopeFactor    = RasterInfo.DepthBiasSlopeFactor,
        .lineWidth               = RasterInfo.LineWidth};

    //////////      Multisample     ///////////////////////
    const auto& MultiSampleInfo = CreateInfo.MultiSample;
    vk::PipelineMultisampleStateCreateInfo MultisampleStateInfo {
        .rasterizationSamples  = static_cast<vk::SampleCountFlagBits>(MultiSampleInfo.RasterizationSampleCount),
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
        .front                 = Convert::StencilOpState(DepthStencilInfo.Front),
        .back                  = Convert::StencilOpState(DepthStencilInfo.Back),
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

    const auto [Result, Pipeline] =
        m_LogicalDevice.createGraphicsPipeline(nullptr,
                                               {.stageCount          = static_cast<uint32_t>(ShaderStageInfo.size()),
                                                .pStages             = ShaderStageInfo.data(),
                                                .pVertexInputState   = &VertexInput,
                                                .pInputAssemblyState = &InputAssemblyInfo,
                                                .pTessellationState  = &TessellationStateInfo,
                                                .pViewportState      = &ViewPortStateInfo,
                                                .pRasterizationState = &RasterizationStateInfo,
                                                .pMultisampleState   = &MultisampleStateInfo,
                                                .pDepthStencilState  = &DepthStencilStateInfo,
                                                .pColorBlendState    = &ColorBlendStateInfo,
                                                .pDynamicState       = &DynamicStateCreateInfo,
                                                .layout = m_ResourceManager.m_PipelineLayout[CreateInfo.PipelineLayout],
                                                .renderPass = m_RenderPass[CreateInfo.RenderPass]});

    for (const auto& ShaderStage : ShaderStageInfo)
    {
        m_LogicalDevice.destroyShaderModule(ShaderStage.module);
    }

    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create graphics pipeline");

    // always true
    return m_Pipeline.emplace(Name, Vulkan::Pipeline {Pipeline}).second;

Failed:
    for (const auto& ShaderStage : ShaderStageInfo)
    {
        m_LogicalDevice.destroyShaderModule(ShaderStage.module);
    }
    return false;
}

bool PipelineManager::RemovePipeline(const std::string& Name) { return m_Pipeline.erase(Name) == 1; }

//////////      Renderpass     ///////////////////////
bool PipelineManager::CreateRenderPass(RenderPassCreateInfo CreateInfo, std::string Name) {}
bool PipelineManager::RemoveRenderPass(const std::string& Name) { m_RenderPass.erase(Name); }

Vulkan::ResourceManager& PipelineManager::ResourceManager() { return m_ResourceManager; }

PipelineManager::PipelineManager(vk::Device D) : m_ResourceManager(D), m_LogicalDevice(D) {}

PipelineManager::~PipelineManager()
{

    for (const auto& P : m_Pipeline | std::ranges::views::values)
    {
        m_LogicalDevice.destroyPipeline(P.m_Pipeline);
    }

    for (const auto& R : m_RenderPass | std::ranges::views::values)
    {
        m_LogicalDevice.destroyRenderPass(R);
    }
}
} // namespace VT::Vulkan
