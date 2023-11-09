//#include "Pipeline.h"
//
//
//namespace VT
//{
//	Pipeline::Pipeline(const std::string& VertexShaderPath, const std::string& FragmentShaderPath)
//	{
//		// createPipeline(VertexShaderPath, FragmentShaderPath);
//	}
//
//
//	std::vector<char> Pipeline::readFile(const std::string& FilePath)
//	{
//		std::ifstream File(FilePath, std::ios::ate | std::ios::binary);
//
//		if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath);
//
//		const auto FileSize = static_cast<size_t>(File.tellg());
//		std::vector<char> Shader(FileSize);
//
//		File.seekg(0);
//		File.read(Shader.data(), FileSize);
//
//		File.close();
//		return Shader;
//	}
//
//	std::vector<std::uint32_t> Pipeline::compile(const ShaderFile& File)
//	{
//		std::unique_ptr<IDxcUtils> DXC_Utils;
//		std::unique_ptr<IDxcCompiler3> DXC_Compiler;
//		DxcCreateInstance(CLSID_DxcUtils, DXC_Compiler);
//		DxcCreateInstance(CLSID_DxcUtils, DXC_Utils);
//
//		auto Src{ readFile(std::string(File.FileLocation)) };
//		DxcBuffer SrcBuff
//		{
//			.Ptr = &Src,
//			.Size = static_cast<std::uint32_t>(Src.size()),
//			.Encoding = 0
//		};
//
//		std::unique_ptr<IDxcResult> CompileResult;
//		DXC_Compiler->Compile(&SrcBuff, File.CL_Args, static_cast<std::uint32_t>(File.CL_Args.size()), nullptr, CompileResult);
//
//		std::unique_ptr<IDxcBlob> ShaderObj;
//		CompileResult->GetOutput(DXC_OUT_OBJECT, &ShaderObj);
//
//		return std::vector<std::uint32_t>(*ShaderObj->GetBufferPointer());
//	}
//
//}