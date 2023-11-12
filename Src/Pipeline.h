#pragma once

#include "Compiler.h"

namespace VT
{

	class Pipeline
	{
	public:
		Pipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath);

		std::vector<char> readFile(const std::string& FilePath);

		std::vector<std::uint32_t> compile(ShaderFileInfo);

	private:

	};
}