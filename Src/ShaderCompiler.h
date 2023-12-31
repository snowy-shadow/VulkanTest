#pragma once

#include "DXC_Compiler.h"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <unordered_set>

namespace VT
{
	class ShaderCompiler
	{
	public:
		ShaderCompiler() = default;

		void setWorkingDir(std::filesystem::path DestFolderPath = "../spv");

		[[nodiscard]]
		vk::ShaderModule createShaderModule(DXC_ShaderFileInfo FileInfo, vk::Device& Device) const;

		void appendShaderStage(vk::PipelineShaderStageCreateInfo Info);

	private:
		// in development, do not use
		/*[[nodiscard]]
		std::vector<std::byte> compileFile(DXC_ShaderFileInfo FileInfo);*/

		[[nodiscard]]
		std::vector<std::byte> fileToSpv(DXC_ShaderFileInfo FileInfo) const;

		std::unordered_map<std::string, std::string> m_ShaderLog;
		std::filesystem::path m_WorkingDir{};

		// modify at your own risk
		std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStages;
	};
}