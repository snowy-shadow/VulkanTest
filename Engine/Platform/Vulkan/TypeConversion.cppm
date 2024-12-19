module;
#include <vulkan/vulkan.hpp>
export module VT.Platform.Vulkan.TypeConversion;

import VT.RendererStructure;

export namespace VT
{
constexpr vk::StencilOpState ToVk(const VT::StencilOpState& State)
{
    return {.failOp      = static_cast<vk::StencilOp>(State.FailOp),
            .passOp      = static_cast<vk::StencilOp>(State.PassOp),
            .depthFailOp = static_cast<vk::StencilOp>(State.DepthFailOp),
            .compareOp   = static_cast<vk::CompareOp>(State.CompareOp),
            .compareMask = State.CompareMask,
            .writeMask   = State.WriteMask,
            .reference   = State.Reference};
}
}; // namespace VT
