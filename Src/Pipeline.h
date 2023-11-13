#pragma once

#include "DXC_Compiler.h"

#include <unordered_set>

namespace VT
{
	class Pipeline
	{
	public:
		Pipeline() = default;

		void setWorkingDir(std::filesystem::path DestFolderPath = "../spv");

		[[nodiscard]]
		std::vector<std::byte> compileFile(DXC_ShaderFileInfo FileInfo);

		[[nodiscard]]
		std::vector<std::byte> forceCompileFile(DXC_ShaderFileInfo FileInfo);

	private:

		DXC_Compiler m_Compiler;

		std::string m_ShaderLog{nullptr};
		std::filesystem::path m_WorkingDir{ "../spv" };
	};
}