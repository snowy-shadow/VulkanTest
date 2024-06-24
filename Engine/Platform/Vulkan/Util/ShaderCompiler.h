#pragma once

#include "DXC_Compiler.h"
#include "GLSL_Compiler.h"

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


namespace VT
{
	class ShaderCompiler
	{
	public:
		ShaderCompiler() = default;

		void setWorkingDir(std::filesystem::path DestFolderPath = "../spv");

		std::vector<std::vector<std::byte>> compileShaders(std::span<const File::DXC_ShaderFileInfo> ShaderInfos) const;

		// std::vector<std::vector<uint32_t>> compileShaders(const std::vector<File::ShadercFileInfo>& ShaderInfos) const;

		std::vector<std::byte> compileShader(const File::DXC_ShaderFileInfo& ShaderInfo) const;

		// std::vector<uint32_t> compileShader(File::ShadercFileInfo ShaderInfos) const;

	private:
		// in development, do not use
		/*[[nodiscard]]
		std::vector<std::byte> compileFile(DXC_ShaderFileInfo FileInfo);*/

		[[nodiscard]]
		std::vector<std::byte> fileToSpv(File::DXC_ShaderFileInfo FileInfo) const;

		std::unordered_map<std::string, std::string> m_ShaderLog;
		std::filesystem::path m_WorkingDir{};
	};
}