module;
#include <vulkan/vulkan.h>
#include <type_traits>
export module VT.RendererEnum;

export namespace VT
{
enum class VT_ENGINE_EXPORT GraphicsAPI
{
    eNone,
    eVulkan
};

enum class PrimitiveTopology
{
    ePointList                  = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    eLineList                   = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    eLineStrip                  = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    eTriangleList               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    eTriangleStrip              = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    eTriangleFan                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    eLineListWithAdjacency      = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
    eLineStripWithAdjacency     = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
    eTriangleListWithAdjacency  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
    eTriangleStripWithAdjacency = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
    ePatchList                  = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
};

enum class PolygonMode
{
    eFill            = VK_POLYGON_MODE_FILL,
    eLine            = VK_POLYGON_MODE_LINE,
    ePoint           = VK_POLYGON_MODE_POINT,
    eFillRectangleNV = VK_POLYGON_MODE_FILL_RECTANGLE_NV
};

enum class CullMode
{
    eNone         = VK_CULL_MODE_NONE,
    eFront        = VK_CULL_MODE_FRONT_BIT,
    eBack         = VK_CULL_MODE_BACK_BIT,
    eFrontAndBack = VK_CULL_MODE_FRONT_AND_BACK
};

enum class FrontFace
{
    eCounterClockwise = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    eClockwise        = VK_FRONT_FACE_CLOCKWISE
};

enum class SampleCount
{
    e1  = VK_SAMPLE_COUNT_1_BIT,
    e2  = VK_SAMPLE_COUNT_2_BIT,
    e4  = VK_SAMPLE_COUNT_4_BIT,
    e8  = VK_SAMPLE_COUNT_8_BIT,
    e16 = VK_SAMPLE_COUNT_16_BIT,
    e32 = VK_SAMPLE_COUNT_32_BIT,
    e64 = VK_SAMPLE_COUNT_64_BIT
};

enum class CompareOp
{
    eNever          = VK_COMPARE_OP_NEVER,
    eLess           = VK_COMPARE_OP_LESS,
    eEqual          = VK_COMPARE_OP_EQUAL,
    eLessOrEqual    = VK_COMPARE_OP_LESS_OR_EQUAL,
    eGreater        = VK_COMPARE_OP_GREATER,
    eNotEqual       = VK_COMPARE_OP_NOT_EQUAL,
    eGreaterOrEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
    eAlways         = VK_COMPARE_OP_ALWAYS
};

enum class StencilOp
{
    eKeep              = VK_STENCIL_OP_KEEP,
    eZero              = VK_STENCIL_OP_ZERO,
    eReplace           = VK_STENCIL_OP_REPLACE,
    eIncrementAndClamp = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
    eDecrementAndClamp = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
    eInvert            = VK_STENCIL_OP_INVERT,
    eIncrementAndWrap  = VK_STENCIL_OP_INCREMENT_AND_WRAP,
    eDecrementAndWrap  = VK_STENCIL_OP_DECREMENT_AND_WRAP
};

enum class LogicOp
{
    eClear        = VK_LOGIC_OP_CLEAR,
    eAnd          = VK_LOGIC_OP_AND,
    eAndReverse   = VK_LOGIC_OP_AND_REVERSE,
    eCopy         = VK_LOGIC_OP_COPY,
    eAndInverted  = VK_LOGIC_OP_AND_INVERTED,
    eNoOp         = VK_LOGIC_OP_NO_OP,
    eXor          = VK_LOGIC_OP_XOR,
    eOr           = VK_LOGIC_OP_OR,
    eNor          = VK_LOGIC_OP_NOR,
    eEquivalent   = VK_LOGIC_OP_EQUIVALENT,
    eInvert       = VK_LOGIC_OP_INVERT,
    eOrReverse    = VK_LOGIC_OP_OR_REVERSE,
    eCopyInverted = VK_LOGIC_OP_COPY_INVERTED,
    eOrInverted   = VK_LOGIC_OP_OR_INVERTED,
    eNand         = VK_LOGIC_OP_NAND,
    eSet          = VK_LOGIC_OP_SET
};

enum class AttachmentLoadOp
{
    eLoad     = VK_ATTACHMENT_LOAD_OP_LOAD,
    eClear    = VK_ATTACHMENT_LOAD_OP_CLEAR,
    eDontCare = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
    eNoneKHR  = VK_ATTACHMENT_LOAD_OP_NONE_KHR,
    eNoneEXT  = VK_ATTACHMENT_LOAD_OP_NONE_EXT
};

enum class AttachmentStoreOp
{
    eStore    = VK_ATTACHMENT_STORE_OP_STORE,
    eDontCare = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    eNone     = VK_ATTACHMENT_STORE_OP_NONE,
    eNoneEXT  = VK_ATTACHMENT_STORE_OP_NONE_EXT,
    eNoneKHR  = VK_ATTACHMENT_STORE_OP_NONE_KHR,
    eNoneQCOM = VK_ATTACHMENT_STORE_OP_NONE_QCOM
};

enum class ImageLayout
{
    eUndefined                                = VK_IMAGE_LAYOUT_UNDEFINED,
    eGeneral                                  = VK_IMAGE_LAYOUT_GENERAL,
    eColorAttachmentOptimal                   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    eDepthStencilAttachmentOptimal            = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    eDepthStencilReadOnlyOptimal              = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    eShaderReadOnlyOptimal                    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    eTransferSrcOptimal                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    eTransferDstOptimal                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    ePreinitialized                           = VK_IMAGE_LAYOUT_PREINITIALIZED,
    eDepthReadOnlyStencilAttachmentOptimal    = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
    eDepthReadOnlyStencilAttachmentOptimalKHR = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR,
    eDepthAttachmentStencilReadOnlyOptimal    = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
    eDepthAttachmentStencilReadOnlyOptimalKHR = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR,
    eDepthAttachmentOptimal                   = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    eDepthAttachmentOptimalKHR                = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR,
    eDepthReadOnlyOptimal                     = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
    eDepthReadOnlyOptimalKHR                  = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR,
    eStencilAttachmentOptimal                 = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
    eStencilAttachmentOptimalKHR              = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR,
    eStencilReadOnlyOptimal                   = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
    eStencilReadOnlyOptimalKHR                = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR,
    eReadOnlyOptimal                          = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    eReadOnlyOptimalKHR                       = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR,
    eAttachmentOptimal                        = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
    eAttachmentOptimalKHR                     = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
    ePresentSrcKHR                            = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    eVideoDecodeDstKHR                        = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR,
    eVideoDecodeSrcKHR                        = VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR,
    eVideoDecodeDpbKHR                        = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR,
    eSharedPresentKHR                         = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
    eFragmentDensityMapOptimalEXT             = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT,
    eFragmentShadingRateAttachmentOptimalKHR  = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
    eShadingRateOptimalNV                     = VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV,
    eRenderingLocalReadKHR                    = VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR,
    eVideoEncodeDstKHR                        = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR,
    eVideoEncodeSrcKHR                        = VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR,
    eVideoEncodeDpbKHR                        = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR,
    eAttachmentFeedbackLoopOptimalEXT         = VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT
};
enum class DescriptorType
{
    eSampler                  = VK_DESCRIPTOR_TYPE_SAMPLER,
    eCombinedImageSampler     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    eSampledImage             = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    eStorageImage             = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    eUniformTexelBuffer       = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    eStorageTexelBuffer       = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    eUniformBuffer            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    eStorageBuffer            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    eUniformBufferDynamic     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    eStorageBufferDynamic     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    eInputAttachment          = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    eInlineUniformBlock       = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK,
    eInlineUniformBlockEXT    = VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT,
    eAccelerationStructureKHR = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
    eAccelerationStructureNV  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV,
    eSampleWeightImageQCOM    = VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM,
    eBlockMatchImageQCOM      = VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM,
    eMutableEXT               = VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
    eMutableVALVE             = VK_DESCRIPTOR_TYPE_MUTABLE_VALVE
};

enum class PipelineBindPoint
{
    eGraphics             = VK_PIPELINE_BIND_POINT_GRAPHICS,
    eCompute              = VK_PIPELINE_BIND_POINT_COMPUTE,
    eRayTracingKHR        = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
    eRayTracingNV         = VK_PIPELINE_BIND_POINT_RAY_TRACING_NV,
    eSubpassShadingHUAWEI = VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI
};

enum class Format
{
    eUndefined                               = VK_FORMAT_UNDEFINED,
    eR4G4UnormPack8                          = VK_FORMAT_R4G4_UNORM_PACK8,
    eR4G4B4A4UnormPack16                     = VK_FORMAT_R4G4B4A4_UNORM_PACK16,
    eB4G4R4A4UnormPack16                     = VK_FORMAT_B4G4R4A4_UNORM_PACK16,
    eR5G6B5UnormPack16                       = VK_FORMAT_R5G6B5_UNORM_PACK16,
    eB5G6R5UnormPack16                       = VK_FORMAT_B5G6R5_UNORM_PACK16,
    eR5G5B5A1UnormPack16                     = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
    eB5G5R5A1UnormPack16                     = VK_FORMAT_B5G5R5A1_UNORM_PACK16,
    eA1R5G5B5UnormPack16                     = VK_FORMAT_A1R5G5B5_UNORM_PACK16,
    eR8Unorm                                 = VK_FORMAT_R8_UNORM,
    eR8Snorm                                 = VK_FORMAT_R8_SNORM,
    eR8Uscaled                               = VK_FORMAT_R8_USCALED,
    eR8Sscaled                               = VK_FORMAT_R8_SSCALED,
    eR8Uint                                  = VK_FORMAT_R8_UINT,
    eR8Sint                                  = VK_FORMAT_R8_SINT,
    eR8Srgb                                  = VK_FORMAT_R8_SRGB,
    eR8G8Unorm                               = VK_FORMAT_R8G8_UNORM,
    eR8G8Snorm                               = VK_FORMAT_R8G8_SNORM,
    eR8G8Uscaled                             = VK_FORMAT_R8G8_USCALED,
    eR8G8Sscaled                             = VK_FORMAT_R8G8_SSCALED,
    eR8G8Uint                                = VK_FORMAT_R8G8_UINT,
    eR8G8Sint                                = VK_FORMAT_R8G8_SINT,
    eR8G8Srgb                                = VK_FORMAT_R8G8_SRGB,
    eR8G8B8Unorm                             = VK_FORMAT_R8G8B8_UNORM,
    eR8G8B8Snorm                             = VK_FORMAT_R8G8B8_SNORM,
    eR8G8B8Uscaled                           = VK_FORMAT_R8G8B8_USCALED,
    eR8G8B8Sscaled                           = VK_FORMAT_R8G8B8_SSCALED,
    eR8G8B8Uint                              = VK_FORMAT_R8G8B8_UINT,
    eR8G8B8Sint                              = VK_FORMAT_R8G8B8_SINT,
    eR8G8B8Srgb                              = VK_FORMAT_R8G8B8_SRGB,
    eB8G8R8Unorm                             = VK_FORMAT_B8G8R8_UNORM,
    eB8G8R8Snorm                             = VK_FORMAT_B8G8R8_SNORM,
    eB8G8R8Uscaled                           = VK_FORMAT_B8G8R8_USCALED,
    eB8G8R8Sscaled                           = VK_FORMAT_B8G8R8_SSCALED,
    eB8G8R8Uint                              = VK_FORMAT_B8G8R8_UINT,
    eB8G8R8Sint                              = VK_FORMAT_B8G8R8_SINT,
    eB8G8R8Srgb                              = VK_FORMAT_B8G8R8_SRGB,
    eR8G8B8A8Unorm                           = VK_FORMAT_R8G8B8A8_UNORM,
    eR8G8B8A8Snorm                           = VK_FORMAT_R8G8B8A8_SNORM,
    eR8G8B8A8Uscaled                         = VK_FORMAT_R8G8B8A8_USCALED,
    eR8G8B8A8Sscaled                         = VK_FORMAT_R8G8B8A8_SSCALED,
    eR8G8B8A8Uint                            = VK_FORMAT_R8G8B8A8_UINT,
    eR8G8B8A8Sint                            = VK_FORMAT_R8G8B8A8_SINT,
    eR8G8B8A8Srgb                            = VK_FORMAT_R8G8B8A8_SRGB,
    eB8G8R8A8Unorm                           = VK_FORMAT_B8G8R8A8_UNORM,
    eB8G8R8A8Snorm                           = VK_FORMAT_B8G8R8A8_SNORM,
    eB8G8R8A8Uscaled                         = VK_FORMAT_B8G8R8A8_USCALED,
    eB8G8R8A8Sscaled                         = VK_FORMAT_B8G8R8A8_SSCALED,
    eB8G8R8A8Uint                            = VK_FORMAT_B8G8R8A8_UINT,
    eB8G8R8A8Sint                            = VK_FORMAT_B8G8R8A8_SINT,
    eB8G8R8A8Srgb                            = VK_FORMAT_B8G8R8A8_SRGB,
    eA8B8G8R8UnormPack32                     = VK_FORMAT_A8B8G8R8_UNORM_PACK32,
    eA8B8G8R8SnormPack32                     = VK_FORMAT_A8B8G8R8_SNORM_PACK32,
    eA8B8G8R8UscaledPack32                   = VK_FORMAT_A8B8G8R8_USCALED_PACK32,
    eA8B8G8R8SscaledPack32                   = VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
    eA8B8G8R8UintPack32                      = VK_FORMAT_A8B8G8R8_UINT_PACK32,
    eA8B8G8R8SintPack32                      = VK_FORMAT_A8B8G8R8_SINT_PACK32,
    eA8B8G8R8SrgbPack32                      = VK_FORMAT_A8B8G8R8_SRGB_PACK32,
    eA2R10G10B10UnormPack32                  = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    eA2R10G10B10SnormPack32                  = VK_FORMAT_A2R10G10B10_SNORM_PACK32,
    eA2R10G10B10UscaledPack32                = VK_FORMAT_A2R10G10B10_USCALED_PACK32,
    eA2R10G10B10SscaledPack32                = VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
    eA2R10G10B10UintPack32                   = VK_FORMAT_A2R10G10B10_UINT_PACK32,
    eA2R10G10B10SintPack32                   = VK_FORMAT_A2R10G10B10_SINT_PACK32,
    eA2B10G10R10UnormPack32                  = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
    eA2B10G10R10SnormPack32                  = VK_FORMAT_A2B10G10R10_SNORM_PACK32,
    eA2B10G10R10UscaledPack32                = VK_FORMAT_A2B10G10R10_USCALED_PACK32,
    eA2B10G10R10SscaledPack32                = VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
    eA2B10G10R10UintPack32                   = VK_FORMAT_A2B10G10R10_UINT_PACK32,
    eA2B10G10R10SintPack32                   = VK_FORMAT_A2B10G10R10_SINT_PACK32,
    eR16Unorm                                = VK_FORMAT_R16_UNORM,
    eR16Snorm                                = VK_FORMAT_R16_SNORM,
    eR16Uscaled                              = VK_FORMAT_R16_USCALED,
    eR16Sscaled                              = VK_FORMAT_R16_SSCALED,
    eR16Uint                                 = VK_FORMAT_R16_UINT,
    eR16Sint                                 = VK_FORMAT_R16_SINT,
    eR16Sfloat                               = VK_FORMAT_R16_SFLOAT,
    eR16G16Unorm                             = VK_FORMAT_R16G16_UNORM,
    eR16G16Snorm                             = VK_FORMAT_R16G16_SNORM,
    eR16G16Uscaled                           = VK_FORMAT_R16G16_USCALED,
    eR16G16Sscaled                           = VK_FORMAT_R16G16_SSCALED,
    eR16G16Uint                              = VK_FORMAT_R16G16_UINT,
    eR16G16Sint                              = VK_FORMAT_R16G16_SINT,
    eR16G16Sfloat                            = VK_FORMAT_R16G16_SFLOAT,
    eR16G16B16Unorm                          = VK_FORMAT_R16G16B16_UNORM,
    eR16G16B16Snorm                          = VK_FORMAT_R16G16B16_SNORM,
    eR16G16B16Uscaled                        = VK_FORMAT_R16G16B16_USCALED,
    eR16G16B16Sscaled                        = VK_FORMAT_R16G16B16_SSCALED,
    eR16G16B16Uint                           = VK_FORMAT_R16G16B16_UINT,
    eR16G16B16Sint                           = VK_FORMAT_R16G16B16_SINT,
    eR16G16B16Sfloat                         = VK_FORMAT_R16G16B16_SFLOAT,
    eR16G16B16A16Unorm                       = VK_FORMAT_R16G16B16A16_UNORM,
    eR16G16B16A16Snorm                       = VK_FORMAT_R16G16B16A16_SNORM,
    eR16G16B16A16Uscaled                     = VK_FORMAT_R16G16B16A16_USCALED,
    eR16G16B16A16Sscaled                     = VK_FORMAT_R16G16B16A16_SSCALED,
    eR16G16B16A16Uint                        = VK_FORMAT_R16G16B16A16_UINT,
    eR16G16B16A16Sint                        = VK_FORMAT_R16G16B16A16_SINT,
    eR16G16B16A16Sfloat                      = VK_FORMAT_R16G16B16A16_SFLOAT,
    eR32Uint                                 = VK_FORMAT_R32_UINT,
    eR32Sint                                 = VK_FORMAT_R32_SINT,
    eR32Sfloat                               = VK_FORMAT_R32_SFLOAT,
    eR32G32Uint                              = VK_FORMAT_R32G32_UINT,
    eR32G32Sint                              = VK_FORMAT_R32G32_SINT,
    eR32G32Sfloat                            = VK_FORMAT_R32G32_SFLOAT,
    eR32G32B32Uint                           = VK_FORMAT_R32G32B32_UINT,
    eR32G32B32Sint                           = VK_FORMAT_R32G32B32_SINT,
    eR32G32B32Sfloat                         = VK_FORMAT_R32G32B32_SFLOAT,
    eR32G32B32A32Uint                        = VK_FORMAT_R32G32B32A32_UINT,
    eR32G32B32A32Sint                        = VK_FORMAT_R32G32B32A32_SINT,
    eR32G32B32A32Sfloat                      = VK_FORMAT_R32G32B32A32_SFLOAT,
    eR64Uint                                 = VK_FORMAT_R64_UINT,
    eR64Sint                                 = VK_FORMAT_R64_SINT,
    eR64Sfloat                               = VK_FORMAT_R64_SFLOAT,
    eR64G64Uint                              = VK_FORMAT_R64G64_UINT,
    eR64G64Sint                              = VK_FORMAT_R64G64_SINT,
    eR64G64Sfloat                            = VK_FORMAT_R64G64_SFLOAT,
    eR64G64B64Uint                           = VK_FORMAT_R64G64B64_UINT,
    eR64G64B64Sint                           = VK_FORMAT_R64G64B64_SINT,
    eR64G64B64Sfloat                         = VK_FORMAT_R64G64B64_SFLOAT,
    eR64G64B64A64Uint                        = VK_FORMAT_R64G64B64A64_UINT,
    eR64G64B64A64Sint                        = VK_FORMAT_R64G64B64A64_SINT,
    eR64G64B64A64Sfloat                      = VK_FORMAT_R64G64B64A64_SFLOAT,
    eB10G11R11UfloatPack32                   = VK_FORMAT_B10G11R11_UFLOAT_PACK32,
    eE5B9G9R9UfloatPack32                    = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
    eD16Unorm                                = VK_FORMAT_D16_UNORM,
    eX8D24UnormPack32                        = VK_FORMAT_X8_D24_UNORM_PACK32,
    eD32Sfloat                               = VK_FORMAT_D32_SFLOAT,
    eS8Uint                                  = VK_FORMAT_S8_UINT,
    eD16UnormS8Uint                          = VK_FORMAT_D16_UNORM_S8_UINT,
    eD24UnormS8Uint                          = VK_FORMAT_D24_UNORM_S8_UINT,
    eD32SfloatS8Uint                         = VK_FORMAT_D32_SFLOAT_S8_UINT,
    eBc1RgbUnormBlock                        = VK_FORMAT_BC1_RGB_UNORM_BLOCK,
    eBc1RgbSrgbBlock                         = VK_FORMAT_BC1_RGB_SRGB_BLOCK,
    eBc1RgbaUnormBlock                       = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    eBc1RgbaSrgbBlock                        = VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
    eBc2UnormBlock                           = VK_FORMAT_BC2_UNORM_BLOCK,
    eBc2SrgbBlock                            = VK_FORMAT_BC2_SRGB_BLOCK,
    eBc3UnormBlock                           = VK_FORMAT_BC3_UNORM_BLOCK,
    eBc3SrgbBlock                            = VK_FORMAT_BC3_SRGB_BLOCK,
    eBc4UnormBlock                           = VK_FORMAT_BC4_UNORM_BLOCK,
    eBc4SnormBlock                           = VK_FORMAT_BC4_SNORM_BLOCK,
    eBc5UnormBlock                           = VK_FORMAT_BC5_UNORM_BLOCK,
    eBc5SnormBlock                           = VK_FORMAT_BC5_SNORM_BLOCK,
    eBc6HUfloatBlock                         = VK_FORMAT_BC6H_UFLOAT_BLOCK,
    eBc6HSfloatBlock                         = VK_FORMAT_BC6H_SFLOAT_BLOCK,
    eBc7UnormBlock                           = VK_FORMAT_BC7_UNORM_BLOCK,
    eBc7SrgbBlock                            = VK_FORMAT_BC7_SRGB_BLOCK,
    eEtc2R8G8B8UnormBlock                    = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    eEtc2R8G8B8SrgbBlock                     = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    eEtc2R8G8B8A1UnormBlock                  = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    eEtc2R8G8B8A1SrgbBlock                   = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    eEtc2R8G8B8A8UnormBlock                  = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    eEtc2R8G8B8A8SrgbBlock                   = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    eEacR11UnormBlock                        = VK_FORMAT_EAC_R11_UNORM_BLOCK,
    eEacR11SnormBlock                        = VK_FORMAT_EAC_R11_SNORM_BLOCK,
    eEacR11G11UnormBlock                     = VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
    eEacR11G11SnormBlock                     = VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
    eAstc4x4UnormBlock                       = VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
    eAstc4x4SrgbBlock                        = VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
    eAstc5x4UnormBlock                       = VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
    eAstc5x4SrgbBlock                        = VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
    eAstc5x5UnormBlock                       = VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
    eAstc5x5SrgbBlock                        = VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
    eAstc6x5UnormBlock                       = VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
    eAstc6x5SrgbBlock                        = VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
    eAstc6x6UnormBlock                       = VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
    eAstc6x6SrgbBlock                        = VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
    eAstc8x5UnormBlock                       = VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
    eAstc8x5SrgbBlock                        = VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
    eAstc8x6UnormBlock                       = VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
    eAstc8x6SrgbBlock                        = VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
    eAstc8x8UnormBlock                       = VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
    eAstc8x8SrgbBlock                        = VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
    eAstc10x5UnormBlock                      = VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
    eAstc10x5SrgbBlock                       = VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
    eAstc10x6UnormBlock                      = VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
    eAstc10x6SrgbBlock                       = VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
    eAstc10x8UnormBlock                      = VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
    eAstc10x8SrgbBlock                       = VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
    eAstc10x10UnormBlock                     = VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
    eAstc10x10SrgbBlock                      = VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
    eAstc12x10UnormBlock                     = VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
    eAstc12x10SrgbBlock                      = VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
    eAstc12x12UnormBlock                     = VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
    eAstc12x12SrgbBlock                      = VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
    eG8B8G8R8422Unorm                        = VK_FORMAT_G8B8G8R8_422_UNORM,
    eG8B8G8R8422UnormKHR                     = VK_FORMAT_G8B8G8R8_422_UNORM_KHR,
    eB8G8R8G8422Unorm                        = VK_FORMAT_B8G8R8G8_422_UNORM,
    eB8G8R8G8422UnormKHR                     = VK_FORMAT_B8G8R8G8_422_UNORM_KHR,
    eG8B8R83Plane420Unorm                    = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
    eG8B8R83Plane420UnormKHR                 = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR,
    eG8B8R82Plane420Unorm                    = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
    eG8B8R82Plane420UnormKHR                 = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR,
    eG8B8R83Plane422Unorm                    = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
    eG8B8R83Plane422UnormKHR                 = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR,
    eG8B8R82Plane422Unorm                    = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
    eG8B8R82Plane422UnormKHR                 = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR,
    eG8B8R83Plane444Unorm                    = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
    eG8B8R83Plane444UnormKHR                 = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR,
    eR10X6UnormPack16                        = VK_FORMAT_R10X6_UNORM_PACK16,
    eR10X6UnormPack16KHR                     = VK_FORMAT_R10X6_UNORM_PACK16_KHR,
    eR10X6G10X6Unorm2Pack16                  = VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
    eR10X6G10X6Unorm2Pack16KHR               = VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR,
    eR10X6G10X6B10X6A10X6Unorm4Pack16        = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
    eR10X6G10X6B10X6A10X6Unorm4Pack16KHR     = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR,
    eG10X6B10X6G10X6R10X6422Unorm4Pack16     = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
    eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR  = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR,
    eB10X6G10X6R10X6G10X6422Unorm4Pack16     = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
    eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR  = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR,
    eG10X6B10X6R10X63Plane420Unorm3Pack16    = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
    eG10X6B10X6R10X63Plane420Unorm3Pack16KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR,
    eG10X6B10X6R10X62Plane420Unorm3Pack16    = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
    eG10X6B10X6R10X62Plane420Unorm3Pack16KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR,
    eG10X6B10X6R10X63Plane422Unorm3Pack16    = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
    eG10X6B10X6R10X63Plane422Unorm3Pack16KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR,
    eG10X6B10X6R10X62Plane422Unorm3Pack16    = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
    eG10X6B10X6R10X62Plane422Unorm3Pack16KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR,
    eG10X6B10X6R10X63Plane444Unorm3Pack16    = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
    eG10X6B10X6R10X63Plane444Unorm3Pack16KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR,
    eR12X4UnormPack16                        = VK_FORMAT_R12X4_UNORM_PACK16,
    eR12X4UnormPack16KHR                     = VK_FORMAT_R12X4_UNORM_PACK16_KHR,
    eR12X4G12X4Unorm2Pack16                  = VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
    eR12X4G12X4Unorm2Pack16KHR               = VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR,
    eR12X4G12X4B12X4A12X4Unorm4Pack16        = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
    eR12X4G12X4B12X4A12X4Unorm4Pack16KHR     = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR,
    eG12X4B12X4G12X4R12X4422Unorm4Pack16     = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
    eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR  = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR,
    eB12X4G12X4R12X4G12X4422Unorm4Pack16     = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
    eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR  = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR,
    eG12X4B12X4R12X43Plane420Unorm3Pack16    = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
    eG12X4B12X4R12X43Plane420Unorm3Pack16KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR,
    eG12X4B12X4R12X42Plane420Unorm3Pack16    = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
    eG12X4B12X4R12X42Plane420Unorm3Pack16KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR,
    eG12X4B12X4R12X43Plane422Unorm3Pack16    = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
    eG12X4B12X4R12X43Plane422Unorm3Pack16KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR,
    eG12X4B12X4R12X42Plane422Unorm3Pack16    = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
    eG12X4B12X4R12X42Plane422Unorm3Pack16KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR,
    eG12X4B12X4R12X43Plane444Unorm3Pack16    = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
    eG12X4B12X4R12X43Plane444Unorm3Pack16KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR,
    eG16B16G16R16422Unorm                    = VK_FORMAT_G16B16G16R16_422_UNORM,
    eG16B16G16R16422UnormKHR                 = VK_FORMAT_G16B16G16R16_422_UNORM_KHR,
    eB16G16R16G16422Unorm                    = VK_FORMAT_B16G16R16G16_422_UNORM,
    eB16G16R16G16422UnormKHR                 = VK_FORMAT_B16G16R16G16_422_UNORM_KHR,
    eG16B16R163Plane420Unorm                 = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
    eG16B16R163Plane420UnormKHR              = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR,
    eG16B16R162Plane420Unorm                 = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
    eG16B16R162Plane420UnormKHR              = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR,
    eG16B16R163Plane422Unorm                 = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
    eG16B16R163Plane422UnormKHR              = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR,
    eG16B16R162Plane422Unorm                 = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
    eG16B16R162Plane422UnormKHR              = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR,
    eG16B16R163Plane444Unorm                 = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
    eG16B16R163Plane444UnormKHR              = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR,
    eG8B8R82Plane444Unorm                    = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM,
    eG8B8R82Plane444UnormEXT                 = VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT,
    eG10X6B10X6R10X62Plane444Unorm3Pack16    = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
    eG10X6B10X6R10X62Plane444Unorm3Pack16EXT = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT,
    eG12X4B12X4R12X42Plane444Unorm3Pack16    = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
    eG12X4B12X4R12X42Plane444Unorm3Pack16EXT = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT,
    eG16B16R162Plane444Unorm                 = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM,
    eG16B16R162Plane444UnormEXT              = VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT,
    eA4R4G4B4UnormPack16                     = VK_FORMAT_A4R4G4B4_UNORM_PACK16,
    eA4R4G4B4UnormPack16EXT                  = VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT,
    eA4B4G4R4UnormPack16                     = VK_FORMAT_A4B4G4R4_UNORM_PACK16,
    eA4B4G4R4UnormPack16EXT                  = VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT,
    eAstc4x4SfloatBlock                      = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK,
    eAstc4x4SfloatBlockEXT                   = VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT,
    eAstc5x4SfloatBlock                      = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK,
    eAstc5x4SfloatBlockEXT                   = VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT,
    eAstc5x5SfloatBlock                      = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK,
    eAstc5x5SfloatBlockEXT                   = VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT,
    eAstc6x5SfloatBlock                      = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK,
    eAstc6x5SfloatBlockEXT                   = VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT,
    eAstc6x6SfloatBlock                      = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK,
    eAstc6x6SfloatBlockEXT                   = VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT,
    eAstc8x5SfloatBlock                      = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK,
    eAstc8x5SfloatBlockEXT                   = VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT,
    eAstc8x6SfloatBlock                      = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK,
    eAstc8x6SfloatBlockEXT                   = VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT,
    eAstc8x8SfloatBlock                      = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK,
    eAstc8x8SfloatBlockEXT                   = VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT,
    eAstc10x5SfloatBlock                     = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK,
    eAstc10x5SfloatBlockEXT                  = VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT,
    eAstc10x6SfloatBlock                     = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK,
    eAstc10x6SfloatBlockEXT                  = VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT,
    eAstc10x8SfloatBlock                     = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK,
    eAstc10x8SfloatBlockEXT                  = VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT,
    eAstc10x10SfloatBlock                    = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK,
    eAstc10x10SfloatBlockEXT                 = VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT,
    eAstc12x10SfloatBlock                    = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK,
    eAstc12x10SfloatBlockEXT                 = VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT,
    eAstc12x12SfloatBlock                    = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK,
    eAstc12x12SfloatBlockEXT                 = VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT,
    ePvrtc12BppUnormBlockIMG                 = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
    ePvrtc14BppUnormBlockIMG                 = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
    ePvrtc22BppUnormBlockIMG                 = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
    ePvrtc24BppUnormBlockIMG                 = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
    ePvrtc12BppSrgbBlockIMG                  = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
    ePvrtc14BppSrgbBlockIMG                  = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
    ePvrtc22BppSrgbBlockIMG                  = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
    ePvrtc24BppSrgbBlockIMG                  = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
    eR16G16Sfixed5NV                         = VK_FORMAT_R16G16_SFIXED5_NV,
    eR16G16S105NV                            = VK_FORMAT_R16G16_S10_5_NV,
    eA1B5G5R5UnormPack16KHR                  = VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR,
    eA8UnormKHR                              = VK_FORMAT_A8_UNORM_KHR
};

/* //////////////////////////////////////////////////
*               Flag bits
* //////////////////////////////////////////////////
*/

//////////// IsBitmask flag /////////////////////
template <typename T>
inline constexpr bool IsBitmask = false;

#define ENABLE_BITMASK(type) \
    template <>              \
    inline constexpr bool IsBitmask<type> = true;

///////////// Bitmask to flag ////////////////////////
template <typename T>
concept EnumBitMask = std::is_enum_v<T> && IsBitmask<T>;

template <EnumBitMask BitType>
class Flag
{
public:
    using MaskType = typename std::underlying_type<BitType>::type;

    // constructors
    constexpr Flag() noexcept : m_mask(0) {}

    constexpr Flag(BitType bit) noexcept : m_mask(static_cast<MaskType>(bit)) {}

    constexpr Flag(Flag<BitType> const& rhs) noexcept = default;

    constexpr explicit Flag(MaskType flags) noexcept : m_mask(flags) {}

    auto operator<=>(Flag<BitType> const&) const = default;

    // logical operator
    constexpr bool operator!() const noexcept { return !m_mask; }

    // bitwise operators
    constexpr Flag<BitType> operator&(Flag<BitType> const& rhs) const noexcept
    {
        return Flag<BitType>(m_mask & rhs.m_mask);
    }

    constexpr Flag<BitType> operator|(Flag<BitType> const& rhs) const noexcept
    {
        return Flag<BitType>(m_mask | rhs.m_mask);
    }

    constexpr Flag<BitType> operator^(Flag<BitType> const& rhs) const noexcept
    {
        return Flag<BitType>(m_mask ^ rhs.m_mask);
    }

    // constexpr Flag<BitType> operator~() const noexcept
    //{
    //     return Flag<BitType>(m_mask ^ FlagTraits<BitType>::allFlags.m_mask);
    // }

    // assignment operators
    constexpr Flag<BitType>& operator=(Flag<BitType> const& rhs) noexcept = default;

    constexpr Flag<BitType>& operator|=(Flag<BitType> const& rhs) noexcept
    {
        m_mask |= rhs.m_mask;
        return *this;
    }

    constexpr Flag<BitType>& operator&=(Flag<BitType> const& rhs) noexcept
    {
        m_mask &= rhs.m_mask;
        return *this;
    }

    constexpr Flag<BitType>& operator^=(Flag<BitType> const& rhs) noexcept
    {
        m_mask ^= rhs.m_mask;
        return *this;
    }

    // cast operators
    explicit constexpr operator bool() const noexcept { return !!m_mask; }

    explicit constexpr operator MaskType() const noexcept { return m_mask; }

private:
    MaskType m_mask;
};

// bitwise operators
template <EnumBitMask BitType>
constexpr Flag<BitType> operator&(BitType bit, Flag<BitType> const& flags) noexcept
{
    return flags.operator&(bit);
}

template <EnumBitMask BitType>
constexpr Flag<BitType> operator|(BitType bit, Flag<BitType> const& flags) noexcept
{
    return flags.operator|(bit);
}

template <EnumBitMask BitType>
constexpr Flag<BitType> operator^(BitType bit, Flag<BitType> const& flags) noexcept
{
    return flags.operator^(bit);
}

// bitwise operators on BitType
template <EnumBitMask BitType>
inline constexpr Flag<BitType> operator&(BitType lhs, BitType rhs) noexcept
{
    return Flag<BitType>(lhs) & rhs;
}

template <EnumBitMask BitType>
inline constexpr Flag<BitType> operator|(BitType lhs, BitType rhs) noexcept
{
    return Flag<BitType>(lhs) | rhs;
}

template <EnumBitMask BitType>
inline constexpr Flag<BitType> operator^(BitType lhs, BitType rhs) noexcept
{
    return Flag<BitType>(lhs) ^ rhs;
}

// template <EnumBitMask BitType>
// inline constexpr Flag<BitType> operator~(BitType bit) noexcept
//{
//     return ~(Flag<BitType>(bit));
// }

///////////// Enum ////////////////////////


enum class PipelineStageFlag
{
    eTopOfPipe                        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    eDrawIndirect                     = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
    eVertexInput                      = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
    eVertexShader                     = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
    eTessellationControlShader        = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
    eTessellationEvaluationShader     = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
    eGeometryShader                   = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
    eFragmentShader                   = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    eEarlyFragmentTests               = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    eLateFragmentTests                = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
    eColorAttachmentOutput            = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    eComputeShader                    = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    eTransfer                         = VK_PIPELINE_STAGE_TRANSFER_BIT,
    eBottomOfPipe                     = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    eHost                             = VK_PIPELINE_STAGE_HOST_BIT,
    eAllGraphics                      = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    eAllCommands                      = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
    eNone                             = VK_PIPELINE_STAGE_NONE,
    eNoneKHR                          = VK_PIPELINE_STAGE_NONE_KHR,
    eTransformFeedbackEXT             = VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT,
    eConditionalRenderingEXT          = VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT,
    eAccelerationStructureBuildKHR    = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
    eAccelerationStructureBuildNV     = VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV,
    eRayTracingShaderKHR              = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
    eRayTracingShaderNV               = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV,
    eFragmentDensityProcessEXT        = VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT,
    eFragmentShadingRateAttachmentKHR = VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
    eShadingRateImageNV               = VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV,
    eCommandPreprocessNV              = VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV,
    eTaskShaderEXT                    = VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT,
    eTaskShaderNV                     = VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV,
    eMeshShaderEXT                    = VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT,
    eMeshShaderNV                     = VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV
};

enum class AccessFlagBit
{
    eIndirectCommandRead                  = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
    eIndexRead                            = VK_ACCESS_INDEX_READ_BIT,
    eVertexAttributeRead                  = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
    eUniformRead                          = VK_ACCESS_UNIFORM_READ_BIT,
    eInputAttachmentRead                  = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
    eShaderRead                           = VK_ACCESS_SHADER_READ_BIT,
    eShaderWrite                          = VK_ACCESS_SHADER_WRITE_BIT,
    eColorAttachmentRead                  = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
    eColorAttachmentWrite                 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    eDepthStencilAttachmentRead           = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    eDepthStencilAttachmentWrite          = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    eTransferRead                         = VK_ACCESS_TRANSFER_READ_BIT,
    eTransferWrite                        = VK_ACCESS_TRANSFER_WRITE_BIT,
    eHostRead                             = VK_ACCESS_HOST_READ_BIT,
    eHostWrite                            = VK_ACCESS_HOST_WRITE_BIT,
    eMemoryRead                           = VK_ACCESS_MEMORY_READ_BIT,
    eMemoryWrite                          = VK_ACCESS_MEMORY_WRITE_BIT,
    eNone                                 = VK_ACCESS_NONE,
    eNoneKHR                              = VK_ACCESS_NONE_KHR,
    eTransformFeedbackWriteEXT            = VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT,
    eTransformFeedbackCounterReadEXT      = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT,
    eTransformFeedbackCounterWriteEXT     = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT,
    eConditionalRenderingReadEXT          = VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT,
    eColorAttachmentReadNoncoherentEXT    = VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT,
    eAccelerationStructureReadKHR         = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR,
    eAccelerationStructureReadNV          = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV,
    eAccelerationStructureWriteKHR        = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
    eAccelerationStructureWriteNV         = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV,
    eFragmentDensityMapReadEXT            = VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT,
    eFragmentShadingRateAttachmentReadKHR = VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,
    eShadingRateImageReadNV               = VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV,
    eCommandPreprocessReadNV              = VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV,
    eCommandPreprocessWriteNV             = VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV
};
ENABLE_BITMASK(AccessFlagBit);
using AccessFlag = Flag<AccessFlagBit>;

enum class DependencyFlag
{
    eByRegion        = VK_DEPENDENCY_BY_REGION_BIT,
    eDeviceGroup     = VK_DEPENDENCY_DEVICE_GROUP_BIT,
    eDeviceGroupKHR  = VK_DEPENDENCY_DEVICE_GROUP_BIT_KHR,
    eViewLocal       = VK_DEPENDENCY_VIEW_LOCAL_BIT,
    eViewLocalKHR    = VK_DEPENDENCY_VIEW_LOCAL_BIT_KHR,
    eFeedbackLoopEXT = VK_DEPENDENCY_FEEDBACK_LOOP_BIT_EXT
};

inline constexpr uint32_t SubpassExternal = VK_SUBPASS_EXTERNAL;
enum class ShaderStageFlagBit
{
    eVertex                 = VK_SHADER_STAGE_VERTEX_BIT,
    eTessellationControl    = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    eTessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    eGeometry               = VK_SHADER_STAGE_GEOMETRY_BIT,
    eFragment               = VK_SHADER_STAGE_FRAGMENT_BIT,
    eCompute                = VK_SHADER_STAGE_COMPUTE_BIT,
    eAllGraphics            = VK_SHADER_STAGE_ALL_GRAPHICS,
    eAll                    = VK_SHADER_STAGE_ALL,
    eRaygenKHR              = VK_SHADER_STAGE_RAYGEN_BIT_KHR,
    eRaygenNV               = VK_SHADER_STAGE_RAYGEN_BIT_NV,
    eAnyHitKHR              = VK_SHADER_STAGE_ANY_HIT_BIT_KHR,
    eAnyHitNV               = VK_SHADER_STAGE_ANY_HIT_BIT_NV,
    eClosestHitKHR          = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR,
    eClosestHitNV           = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV,
    eMissKHR                = VK_SHADER_STAGE_MISS_BIT_KHR,
    eMissNV                 = VK_SHADER_STAGE_MISS_BIT_NV,
    eIntersectionKHR        = VK_SHADER_STAGE_INTERSECTION_BIT_KHR,
    eIntersectionNV         = VK_SHADER_STAGE_INTERSECTION_BIT_NV,
    eCallableKHR            = VK_SHADER_STAGE_CALLABLE_BIT_KHR,
    eCallableNV             = VK_SHADER_STAGE_CALLABLE_BIT_NV,
    eTaskEXT                = VK_SHADER_STAGE_TASK_BIT_EXT,
    eTaskNV                 = VK_SHADER_STAGE_TASK_BIT_NV,
    eMeshEXT                = VK_SHADER_STAGE_MESH_BIT_EXT,
    eMeshNV                 = VK_SHADER_STAGE_MESH_BIT_NV,
    eSubpassShadingHUAWEI   = VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI,
    eClusterCullingHUAWEI   = VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI
};
ENABLE_BITMASK(ShaderStageFlagBit);
using ShaderStageFlags = Flag<ShaderStageFlagBit>;
} // namespace VT
