module;
#include "VT_Export"
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
export module VT.RendererStructure;

import VT.Texture;
import VT.Util.DataStructure;
import VT.RendererEnum;
import VT.Buffer;
import VT.Device;
import VT.RenderPass;
import VT.Pipeline;

export namespace VT
{
using ID                             = uint32_t;
constexpr ID InvalidID               = (ID) -1;
constexpr uint32_t InvalidIndex      = (ID) -1;
constexpr uint32_t TextureInvalideID = (ID) -1;

struct VT_ENGINE_EXPORT RenderPacket {};

struct VT_ENGINE_EXPORT UniformCameraData
{
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
};

struct VT_ENGINE_EXPORT UniformObjectData
{
    glm::vec4 DiffuseColor;
};

struct VT_ENGINE_EXPORT GeometryRenderData
{
    ID ID;
    glm::mat4 Model;
    uint32_t TextureCount;
    Texture** pTexture;
};

struct VT_ENGINE_EXPORT DescriptorLayout
{
    uint32_t Binding;
    DescriptorType DescriptorType;
    uint32_t DescriptorCount;
    ShaderStageFlags Stage;
    uint32_t PushConstantSize = 0;

    // handle to native api layout obj. nullptr if such doesn't exist
    virtual void* GetHandle() = 0;
};

struct VT_ENGINE_EXPORT ShaderSpv
{
    std::vector<uint32_t> Spv;
    DescriptorLayout& Layout;
};

struct VT_ENGINE_EXPORT StencilOpState
{
    StencilOp FailOp;
    StencilOp PassOp;
    StencilOp DepthFailOp;
    CompareOp CompareOp;
    uint32_t CompareMask;
    uint32_t WriteMask;
    uint32_t Reference;
};

struct VT_ENGINE_EXPORT TessellationInfo
{
    uint32_t ControlPoint = 0;
};

struct VT_ENGINE_EXPORT PrimitiveInfo
{
    PrimitiveTopology Topology;
    bool PrimitiveRestart;
};

struct VT_ENGINE_EXPORT RasterizationInfo
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

struct VT_ENGINE_EXPORT MultisampleInfo
{
    SampleCount RasterizationSampleCount = SampleCount::e1;
    bool SampleShading;
    float MinSampleShading;
    // To add const VkSampleMask* pSampleMask
    bool AlphaToCoverage;
    bool AlphaToOne;
};

struct VT_ENGINE_EXPORT DepthStencilInfo
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

struct VT_ENGINE_EXPORT ColorBlendInfo
{
    bool LogicOpEnable;
    LogicOp LogicOp;
    uint32_t AttachmentCount;

    float BlendConstants[4];
};

struct VT_ENGINE_EXPORT AttachmentDescription
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

struct VT_ENGINE_EXPORT SubpassDescription
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

struct VT_ENGINE_EXPORT SubpassDependency
{
    uint32_t SrcSubpass = SubpassExternal;
    uint32_t DstSubpass = 0;
    PipelineStageFlag SrcStageMask;
    PipelineStageFlag DstStageMask;
    AccessFlag SrcAccessMask = AccessFlagBit::eNone;
    AccessFlag DstAccessMask = AccessFlagBit::eColorAttachmentWrite | AccessFlagBit::eColorAttachmentRead;
    DependencyFlag DependencyFlags;
};

struct VT_ENGINE_EXPORT RenderpassCreateInfo
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
    RenderPass& Renderpass;
    Device& LogicalDevice;
};
} // namespace VT
