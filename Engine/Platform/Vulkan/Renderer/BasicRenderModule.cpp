#include "BasicRenderModule.h"

namespace VT
{
	bool BasicRenderModule::createGraphicsPipeline(
		std::span<const File::DXC_ShaderFileInfo> ShaderFiles,
		const std::string& GraphicsPipelineName,
		vk::GraphicsPipelineCreateInfo PipelineInfo,
		const std::string& PipelineLayoutName,
		const std::string& RenderPassName)
    {
		// cannot have name collision
		if(!m_LogicalDevice
			|| m_DependencyGraph.has<vk::Pipeline>(GraphicsPipelineName)			// if name collision
			|| !m_DependencyGraph.has<vk::PipelineLayout>(PipelineLayoutName)	// dependent type not found
			|| !m_DependencyGraph.has<vk::RenderPass>(RenderPassName))			// dependent type not found
		{
			return false;
		}

		ShaderCompiler m_ShaderCompiler;
		// compile shaders
		auto ShaderSpvs{ m_ShaderCompiler.compileShaders(ShaderFiles)};

		// Graphics pipline struct
		std::vector<vk::PipelineShaderStageCreateInfo>& ShaderStageInfos = 
			m_DependencyGraph.insert<std::vector<vk::PipelineShaderStageCreateInfo>>(
				{},
				"ShaderModules",
				[&](auto& SSI) {for (const auto& SM : SSI) { m_LogicalDevice.destroyShaderModule(SM.module); }}
		).first;


		// load all shader spv
		for(std::size_t i = 0; i < ShaderSpvs.size(); i++)
		{
			ShaderStageInfos.push_back
			({
				.stage = ShaderFiles[i].Stage,
				.module = m_LogicalDevice.createShaderModule
				(
					{
						.codeSize = ShaderSpvs[i].size(),
						.pCode = reinterpret_cast<uint32_t*>(ShaderSpvs[i].data())
					}
				),

				.pName = "main"
			});
		}

		PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStageInfos.size());
		PipelineInfo.pStages = ShaderStageInfos.data();

		PipelineInfo.renderPass = m_DependencyGraph.get<vk::RenderPass>(RenderPassName);
		PipelineInfo.layout = m_DependencyGraph.get<vk::PipelineLayout>(PipelineLayoutName);

		auto [Result, Pipeline] = m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo);
	
		// check Pipeline result
		if (Result != vk::Result::eSuccess) { return false; }

		return
		(m_DependencyGraph.insert<vk::Pipeline>(std::move(Pipeline), GraphicsPipelineName, [&](const vk::Pipeline& p) { m_LogicalDevice.destroyPipeline(p); }).second &&
			m_DependencyGraph.addDependency<vk::Pipeline, vk::PipelineLayout>(GraphicsPipelineName, PipelineLayoutName) &&
			m_DependencyGraph.addDependency<vk::Pipeline, vk::RenderPass>(GraphicsPipelineName, RenderPassName));
    }

}
