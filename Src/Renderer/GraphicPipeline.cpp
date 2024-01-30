#include "GraphicPipeline.h"

namespace VT
{
	GraphicPipeline::GraphicPipeline()
	{
		m_VertexInputStateInfo =
		{

		};
		m_InputAssemblyInfo =
		{

		};

	}

	vk::GraphicsPipelineCreateInfo GraphicPipeline::getGraphicPipelineInfo(const std::vector<vk::PipelineShaderStageCreateInfo>& m_ShaderStageInfos) const
	{
		return
		{
			.stageCount = static_cast<uint32_t>(m_ShaderStageInfos.size()),
			.pStages = m_ShaderStageInfos.data(),
			.pVertexInputState = &m_VertexInputStateInfo,
			.pInputAssemblyState = &m_InputAssemblyInfo,
			.pTessellationState = &m_TessellationStateInfo,
			.pViewportState = &m_ViewportStateInfo,
			.pRasterizationState = &m_RasterizationStateInfo,
			.pMultisampleState = &m_MultisampleStateInfo,
			.pDepthStencilState = &m_DepthStencilStateInfo,
			.pColorBlendState = &m_ColorBlendStateInfo,
			.pDynamicState = &m_DynamicStateInfo,
		};
	}

}