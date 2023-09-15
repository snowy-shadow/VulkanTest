#include "Pipeline.h"


namespace VT
{
	Pipeline::Pipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath)
	{
		createPipeline(VertexShaderPath, FragmentShaderPath);
	}

	std::vector<char> Pipeline::readFile(const std::string& FilePath)
	{
		std::ifstream File(FilePath, std::ios::ate | std::ios::binary);

		if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath);

		const auto FileSize = static_cast<size_t>(File.tellg());
		std::vector<char> Shader(FileSize);

		File.seekg(0);
		File.read(Shader.data(), FileSize);

		File.close();
		return Shader;
	}
	void Pipeline::createPipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath)
	{
		auto VertexShader = readFile(VertexShaderPath);
		auto FragmentSahder{ readFile(FragmentShaderPath) };


	}
	void Pipeline::compileShader() const
	{
	}
}