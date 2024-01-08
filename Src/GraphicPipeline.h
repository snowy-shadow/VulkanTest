#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include <vector>

namespace VT
{
	struct GraphicPipeline
	{
		GraphicPipeline();

		std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStageInfos{};

		vk::PipelineVertexInputStateCreateInfo m_VertexInputStateInfo{};
		vk::PipelineInputAssemblyStateCreateInfo m_InputAssemblyInfo{};
		vk::PipelineTessellationStateCreateInfo m_TessellationStateInfo{};
		vk::PipelineViewportStateCreateInfo m_ViewportStateInfo{};
		vk::PipelineRasterizationStateCreateInfo m_RasterizationStateInfo{};
		vk::PipelineMultisampleStateCreateInfo m_MultisampleStateInfo{};
		vk::PipelineDepthStencilStateCreateInfo m_DepthStencilStateInfo{};
		vk::PipelineColorBlendStateCreateInfo m_ColorBlendStateInfo{};
		vk::PipelineDynamicStateCreateInfo m_DynamicStateInfo{};

		[[nodiscard]]
		vk::GraphicsPipelineCreateInfo getGraphicPipelineInfo() const;
	};
}