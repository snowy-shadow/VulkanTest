#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
namespace File { struct DXC_ShaderFileInfo; }
class BasicRenderModule
{
	bool createPipelineLayout(std::string Name, const vk::PipelineLayoutCreateInfo& LayoutInfo);
	/*
	 * Creates graphics pipeline
	 * Name - Name of pipeline
	 * ShaderFiles - files to compile to shaders
	 * LayoutInfo - PipelineLayoutInfo to create layout from
	 * RenderPassInfo - RenderPassCreateInfo to create renderpass from
	 * PipelineInfo - GraphicsPipeline infos, shader stages will be compiled and inserted based on ShaderFiles
	*/
	bool createGraphicsPipeline(
		std::span<const struct File::DXC_ShaderFileInfo> ShaderFiles,
		const std::string& GraphicsPipelineName,
		vk::GraphicsPipelineCreateInfo PipelineInfo,
		const std::string& PipelineLayoutName,
		const std::string& RenderPassName);
private:
    
	vk::Pipeline m_Pipeline;
	vk::PipelineLayout m_PipelineLayout;
};
}
