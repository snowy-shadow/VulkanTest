#pragma once

#include "DXC_Compiler.h"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <unordered_set>

namespace VT
{
	class Pipeline
	{
	public:
		Pipeline() = default;

		//void setWorkingDir(std::filesystem::path DestFolderPath = "../spv");

		[[nodiscard]]
		vk::ShaderModule createShaderModule(DXC_ShaderFileInfo FileInfo, vk::Device& Device) const;

		void appendShaderStage(vk::PipelineShaderStageCreateInfo Info);



	private:
		// in development, do not use
		/*[[nodiscard]]
		std::vector<std::byte> compileFile(DXC_ShaderFileInfo FileInfo);*/

		[[nodiscard]]
		std::vector<std::byte> fileToSpv(DXC_ShaderFileInfo FileInfo) const;

		DXC_Compiler m_Compiler;

	/*	std::string m_ShaderLog{nullptr};
		std::filesystem::path m_WorkingDir{ "../spv" };*/

		// modify at your own risk
		std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStages;
	};
}