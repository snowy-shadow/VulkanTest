module;
#include <vulkan/vulkan.hpp>
#include "VT_Export"

export module VT.Platform.Vulkan.Pipeline;
import VT.Pipeline;
import VT.Util.Datastructure;
import VT.ShaderCompiler;
import VT.RendererType;

export namespace VT::Vulkan
{
struct Pipeline
{
    void Create(const std::vector<vk::PipelineShaderStageCreateInfo>& Shaders,
                vk::PipelineLayoutCreateInfo PipelineLayout,
                vk::PipelineVertexInputStateCreateInfo VertexInput,
                vk::RenderPass Renderpass,
                vk::Device LogicalDevice);
    vk::CommandBuffer Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint);

    void Destroy();
    ~Pipeline();

    vk::Pipeline m_Pipeline;
    vk::PipelineLayout m_Layout;

    vk::Device m_LogicalDevice;
};

struct TessellationInfo
{
    uint32_t ControlPoint;
};

struct PrimitiveInfo
{
    PrimitiveTopology Topology;
    bool PrimitiveRestart;
};

struct RasterizationInfo
{
    bool DepthClamp;
    bool RasterizerDiscard;
    PolygonMode PolygonMode;
    CullMode CullMode;
    FrontFace FrontFace;
    bool DepthBias;
    float DepthBiasConstantFactor;
    float DepthBiasClamp;
    float DepthBiasSlopeFactor;
    float LineWidth;
};


struct MultisampleInfo
{
    SampleCount RasterizationSampleCount = SampleCount::e1;
    bool SampleShading;
    float MinSampleShading;
    // To add const VkSampleMask* pSampleMask
    bool AlphaToCoverage;
    bool AlphaToOne;
};

struct StencilOpState
{
    StencilOp FailOp;
    StencilOp PassOp;
    StencilOp DepthFailOp;
    CompareOp CompareOp;
    uint32_t CompareMask;
    uint32_t WriteMask;
    uint32_t Reference;
};

struct DepthStencilInfo
{
    bool DepthTest;
    bool DepthWrite;
    CompareOp DepthCompareOp;
    bool DepthBoundsTest;
    bool StencilTest;
    StencilOpState Front;
    StencilOpState Back;
    float MinDepthBounds;
    float MaxDepthBounds;
};

struct ColorBlendInfo
{
    bool LogicOpEnable;
    LogicOp LogicOp;
    uint32_t AttachmentCount;

    float BlendConstants[4];
};

struct AttachmentDescription
{
    ImageLayout Use                  = ImageLayout::eUndefined;
    Format Format                    = Format::eUndefined;
    SampleCount Samples              = SampleCount::e1;
    AttachmentLoadOp LoadOp          = AttachmentLoadOp::eLoad;
    AttachmentStoreOp StoreOp        = AttachmentStoreOp::eStore;
    AttachmentLoadOp StencilLoadOp   = AttachmentLoadOp::eLoad;
    AttachmentStoreOp StencilStoreOp = AttachmentStoreOp::eStore;
    ImageLayout InitialLayout        = ImageLayout::eUndefined;
    ImageLayout FinalLayout          = ImageLayout::eUndefined;
};
vk::ShaderStageFlagBits
struct SubpassDescription
{
    PipelineBindPoint PipelineBindPoint;
    uint32_t InputAttachmentCount;
    const AttachmentDescription* pInputAttachments;
    uint32_t ColorAttachmentCount;
    const AttachmentDescription* pColorAttachments;
    const AttachmentDescription* pResolveAttachments;
    const AttachmentDescription* pDepthStencilAttachment;
    uint32_t PreserveAttachmentCount;
    const uint32_t* pPreserveAttachments;
};

struct SubpassDependency
{
    uint32_t SrcSubpass = SubpassExternal;
    uint32_t DstSubpass = 0;
    PipelineStageFlag SrcStageMask;
    PipelineStageFlag DstStageMask;
    AccessFlag SrcAccessMask = AccessFlagBit::eNone;
    AccessFlag DstAccessMask = AccessFlagBit::eColorAttachmentWrite | AccessFlagBit::eColorAttachmentRead;
    DependencyFlag DependencyFlags;
};

struct RenderpassCreateInfo
{
    uint32_t AttachmentCount                  = {};
    const AttachmentDescription* pAttachments = {};
    uint32_t SubpassCount                     = {};
    const SubpassDescription* pSubpasses      = {};
    uint32_t DependencyCount                  = {};
    const SubpassDependency* pDependencies    = {};
    const void* pNext                         = nullptr;
};


struct VT_ENGINE_EXPORT GraphicsPipelineCreateInfo
{
    std::vector<BufferLayout> VertexLayout {};
    std::vector<ShaderSpv> Shader;
    PrimitiveInfo PrimitiveInfo {};
    Optional<TessellationInfo> Tessellation {};
    uint32_t ViewportCount = 1;
    uint32_t ScissorCount  = 1;
    RasterizationInfo Rasterization {};
    MultisampleInfo MultiSample {};
    DepthStencilInfo DepthStencil {};
    ColorBlendInfo ColorBlend {};
    Renderpass Renderpass;
    Device LogicalDevice;

};

class GraphicsPipeline : VT::Pipeline
{
public:
    void Create(GraphicsPipelineCreateInfo);
    void Destroy();

    virtual void Bind() override;

    vk::Pipeline Pipline = VK_NULL_HANDLE;
    vk::Device LogicalDevice = VK_NULL_HANDLE;
};
} // namespace VT::Vulkan
