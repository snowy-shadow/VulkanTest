#include "Compiler.h"

DXC_Compiler::DXC_Compiler()
{
	// Initialize DXC compiler
	if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_DXC_Compiler))))
	{
		throw std::runtime_error("Could not init DXC Compiler");
	}

	// Initialize DXC utility
	if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_DXC_Utils))))
	{
		throw std::runtime_error("Could not init DXC Utiliy");
	}
}

// must receive a CComPtr<IDxcBlob>, or dangling pointer
CComPtr<IDxcBlob> DXC_Compiler::compile(ShaderFileInfo File)
{
	HRESULT HRes;
	File.CL_Args.push_back(File.FileName.c_str());

	CComPtr<IDxcBlobEncoding> SourceBlob;
	HRes = m_DXC_Utils->LoadFile((File.FileLocation + File.FileName).c_str(), reinterpret_cast<uint32_t*>(&File.Encoding), &SourceBlob);
	if (FAILED(HRes)) { throw std::runtime_error("Could not load shader file"); }

	DxcBuffer SrcBuff
	{
		.Ptr = SourceBlob->GetBufferPointer(),
		.Size = SourceBlob->GetBufferSize(),
		.Encoding = File.Encoding
	};

	CComPtr<IDxcResult> CompileResult{ nullptr };
	HRes = m_DXC_Compiler->Compile(&SrcBuff, File.CL_Args.data(), static_cast<std::uint32_t>(File.CL_Args.size()), nullptr, IID_PPV_ARGS(&CompileResult));

	// Output error if compilation failed
	if (FAILED(HRes) && HRes)
	{
		CComPtr<IDxcBlobEncoding> Error;
		HRes = CompileResult->GetErrorBuffer(&Error);

		if (SUCCEEDED(HRes) && Error)
		{
			throw std::runtime_error(std::string("Compilation failed") + static_cast<const char*>(Error->GetBufferPointer()));
		}
	}
	else { CompileResult->GetStatus(&HRes); }

	CComPtr<IDxcBlob> ShaderByteCode;
	CompileResult->GetResult(&ShaderByteCode);

	return ShaderByteCode;
}
