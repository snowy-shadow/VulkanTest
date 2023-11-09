// VulkanTest.cpp : Defines the entry point for the application.
//

//#include "App.h"
//
//int main()
//{
//	try
//	{
//		VT::App App;
//		App.run();
//	}
//	catch (const std::exception& E)
//	{
//		std::cout << E.what();
//	}
//
//	return 0;
//}

#include <bitset>
// #include <Windows.h>
// #include <wrl/client.h>
#include <memory>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <

struct ShaderFile
{
	std::wstring FileLocation;
	std::wstring FileName;
	std::vector<const wchar_t*> CL_Args;
};


std::ostream& operator<<(std::ostream& os, std::byte b)
{
	return os << std::bitset<8>(std::to_integer<int>(b));
}


std::vector<std::byte> readFile(const std::wstring& FilePath)
{
	std::ifstream File(FilePath, std::ios::binary | std::ios::ate);

	if (!File.is_open())  std::cout << "cannot open file";//throw std::runtime_error("Failed to open : " + std::string(FilePath.cbegin(), FilePath.cend()));

	const auto FileSize = static_cast<std::streamsize>(File.tellg());
	std::vector<std::byte> Buffer(FileSize);

	File.seekg(0);
	File.read((char*)Buffer.data(), FileSize);

	File.close();
	return { Buffer.begin(), Buffer.end() };
}

std::vector<std::byte> compileThis1(ShaderFile File)
{
	IDxcUtils* DXC_Utils;
	DxcCreateInstance(CLSID_DxcUtils, CROSS_PLATFORM_UUIDOF(DXC_Utils, "A3F84EAB-0EAA-497E-A39C-EE6ED60B2D84")));




	IDxcCompiler3* DXC_Compiler;
	if(FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DXC_Compiler))))
	{
		std::cout << "cannot create dxc compiler\n";
	}

	auto Src{ readFile(File.FileLocation + File.FileName) };

	// Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
	// uint32_t a = DXC_CP_UTF8;
	// DXC_Utils->LoadFile((File.FileLocation + File.FileName).c_str(), &a, &sourceBlob);

	DxcBuffer SrcBuff
	{
		.Ptr = Src.data(),
		.Size = Src.size(),
		.Encoding = DXC_CP_UTF8
	};

	File.CL_Args.push_back(std::wstring(File.FileName.begin(), File.FileName.end()).c_str());

	IDxcResult* CompileResult;
	DXC_Compiler->Compile(&SrcBuff, File.CL_Args.data(), static_cast<std::uint32_t>(File.CL_Args.size()), nullptr, IID_PPV_ARGS(&CompileResult));

	CComPtr<IDxcBlob> ShaderObj;
	CompileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderObj), nullptr);

	IDxcBlobUtf8* Error;
	CompileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&Error), nullptr);

	std::string es( Error->GetStringPointer() );

	if (Error && Error->GetStringLength() > 0) { std::cout << std::string("Shader failed to compile : ") + es; }

	auto c = static_cast<std::byte*>(ShaderObj->GetBufferPointer());
	std::vector<std::byte> v = { c, c + ShaderObj->GetBufferSize() };

	DXC_Utils->Release();
	DXC_Compiler->Release();
	ShaderObj->Release();
	CompileResult->Release();
	Error->Release();

	return v;
}


int main()
{
	auto r = compileThis1
	({
		.FileLocation = L"D:\\",
		.FileName = L"testa.hlsl",
		.CL_Args = {L"-Zpc", L"-HV 2021", L"-T vs_6_1", L"-E main", L"-spirv", L"-fspv-target-env=vulkan1.3"}
	});

	for (auto p : r)
	{
		std::cout << p << " ";
	}
	std::cout << "\n\n\n\n\n this is suppose to be :\n";

	auto e = readFile(L"D:/testa.spv");
	for (auto p : e)
	{
		std::cout << p << " ";
	}

	std::cout << "\nis equal : " << std::boolalpha << std::equal(r.cbegin(), r.cend(), e.cbegin(), e.cend());

	return 0;
}