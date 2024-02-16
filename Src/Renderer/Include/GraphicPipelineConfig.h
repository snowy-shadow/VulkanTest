#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	struct GraphicPipelineConfig
	{
		vk::PipelineVertexInputStateCreateInfo m_VertexInputStateInfo{};
		vk::PipelineInputAssemblyStateCreateInfo m_InputAssemblyInfo{};
		vk::PipelineTessellationStateCreateInfo m_TessellationStateInfo{};
		vk::PipelineViewportStateCreateInfo m_ViewportStateInfo{};

		// group
		std::vector<vk::Viewport> m_Viewports{};
		std::vector<vk::Rect2D> m_Scissors{};
		vk::PipelineRasterizationStateCreateInfo m_RasterizationStateInfo{};

		vk::PipelineMultisampleStateCreateInfo m_MultisampleStateInfo{};
		vk::PipelineDepthStencilStateCreateInfo m_DepthStencilStateInfo{};

		// group
		std::vector<vk::PipelineColorBlendAttachmentState> m_ColorBlendAttachmentState{};
		vk::PipelineColorBlendStateCreateInfo m_ColorBlendStateInfo{};

		vk::PipelineDynamicStateCreateInfo m_DynamicStateInfo{};

		[[nodiscard]]
		vk::GraphicsPipelineCreateInfo getGraphicPipelineCreateInfo(const std::vector<vk::PipelineShaderStageCreateInfo>& m_ShaderStageInfos) const;
	};
}