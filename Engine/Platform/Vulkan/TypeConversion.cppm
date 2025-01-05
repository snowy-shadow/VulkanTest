module;
#include <vulkan/vulkan.hpp>
export module VT.Platform.Vulkan.TypeConversion;

import VT.RendererStructure;

export namespace VT::Vulkan::Convert
{
constexpr vk::StencilOpState StencilOpState(const VT::StencilOpState& State)
{
    return {.failOp      = static_cast<vk::StencilOp>(State.FailOp),
            .passOp      = static_cast<vk::StencilOp>(State.PassOp),
            .depthFailOp = static_cast<vk::StencilOp>(State.DepthFailOp),
            .compareOp   = static_cast<vk::CompareOp>(State.CompareOp),
            .compareMask = State.CompareMask,
            .writeMask   = State.WriteMask,
            .reference   = State.Reference};
}

constexpr vk::DescriptorSetLayoutBinding DescriptorSetLayoutBinding(const VT::DescriptorLayoutBindingInfo& Layout)
{
    return {.binding         = Layout.Binding,
            .descriptorType  = static_cast<vk::DescriptorType>(Layout.DescriptorType),
            .descriptorCount = Layout.DescriptorCount,
            .stageFlags      = static_cast<vk::ShaderStageFlags>(Layout.Stage.Value())};
}

constexpr vk::DescriptorPoolSize DescriptorPoolSize(const VT::DescriptorPoolSize& PoolSize)
{
    return {.type = static_cast<vk::DescriptorType>(PoolSize.Type),
    .descriptorCount = PoolSize.DescriptorCount};
}
}; // namespace VT::Vulkan::Convert
