#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

#include "Images.h"

namespace VT
{
	class Pipeline
	{
	public:
		Pipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath);

	private:
		static std::vector<char> readFile(const std::string& FilePath);

		void createPipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath);

		void compileShader() const;

		Images m_Images;
	};
}