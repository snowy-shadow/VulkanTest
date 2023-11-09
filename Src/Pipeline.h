#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <dxc/dxcapi.h>


namespace VT
{
	struct ShaderFile
	{
		std::string_view FileLocation;
		std::string_view FileName;
		std::string_view FileType;
		std::vector<std::string_view> CL_Args;
	};

	class Pipeline
	{
	public:
		Pipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath);

		std::vector<char> readFile(const std::string& FilePath);

		std::vector<std::uint32_t> compile(const ShaderFile&);

	private:

	};
}