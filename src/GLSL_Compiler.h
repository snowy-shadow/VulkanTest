#pragma once

#include "File.h"

#include <shaderc/shaderc.hpp>

namespace VT
{
	class GLSL_Compiler
	{
	public:
		GLSL_Compiler();
		std::vector<uint32_t> compile(ShaderFileInfo);

		GLSL_Compiler(const GLSL_Compiler&) = delete;
		GLSL_Compiler& operator = (const GLSL_Compiler&) = delete;
	private:

	};
}
