module;
#include <cstdint>
#include <string>

#ifdef _WIN32
    /*
     * CComPtr
     * MUST COME BEFORE dxcapi.h && AFTER vulkan_hash.hpp
     */
    #include <atlcomcli.h>
#endif

// provides cross platform CComPtr if not WIN32

#include <dxc/dxcapi.h>
#include <shaderc/shaderc.hpp>
#include <filesystem>

#include "EngineMacro.h"

module VT.ShaderCompiler;
import VT.Log;

namespace VT::Shader::GLSL
{
/* =========================================================
 *                      GLSL_Compiler
 * =========================================================
 */
std::vector<uint32_t> Compiler::CompileSpv(ShaderFileInfo FileInfo)
{
    const auto Src{ FileInfo.FileDir / FileInfo.FileName };
    const auto File = File::ReadFile(Src);
    std::string Source {File.cbegin(), File.cend()};

    const shaderc::SpvCompilationResult Module {
        m_Compiler.CompileGlslToSpv(Source, FileInfo.ShaderType, FileInfo.FileName.c_str(), FileInfo.CompileOptions)};

    if (Module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        VT_HALT("Failed to compile {0}, Message : {1}\n ", Src.string(), Module.GetErrorMessage());
    }

    return {Module.cbegin(), Module.cend()};
}
} // namespace VT::Shader::GLSL

namespace VT::Shader::DXC
{
/* =========================================================
 *                      DXC_Compiler
 * =========================================================
 */
Compiler::Compiler()
{
    // Initialize DXC compiler
    if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_Compiler))))
    {
        throw std::runtime_error("Could not init DXC Compiler\n");
    }

    // Initialize DXC utility
    if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_DXC_Utils))))
    {
        throw std::runtime_error("Could not init DXC Utiliy\n");
    }
}

// must receive a CComPtr<IDxcBlob>, or dangling pointer
std::vector<std::byte> Compiler::CompileSpv(ShaderFileInfo& File) const
{
    // parse data
    const std::filesystem::path Src{ File.FileDir / File.FileName };

    File.CL_Args.emplace_back(Src.wstring().c_str());

    // load file
    CComPtr<IDxcBlobEncoding> SourceBlob;
    HRESULT HRes = m_DXC_Utils->LoadFile(Src.wstring().c_str(), static_cast<UINT32*>(&File.Encoding), &SourceBlob);
    if (FAILED(HRes))
    {
        throw std::runtime_error("Could not load shader file : " + Src.string());
    }

    DxcBuffer SrcBuff {
        .Ptr = SourceBlob->GetBufferPointer(), .Size = SourceBlob->GetBufferSize(), .Encoding = File.Encoding};

    // compile
    CComPtr<IDxcResult> CompileResult {nullptr};
    HRes = m_Compiler->Compile(
        &SrcBuff,
        static_cast<LPCWSTR*>(File.CL_Args.data()),
        static_cast<uint32_t>(File.CL_Args.size()),
        nullptr,
        IID_PPV_ARGS(&CompileResult));

    // get compile result
    if (SUCCEEDED(HRes))
    {
        CompileResult->GetStatus(&HRes);
    }

    // Output error if compilation failed
    if (FAILED(HRes) && CompileResult)
    {
        CComPtr<IDxcBlobEncoding> Error;
        HRes = CompileResult->GetErrorBuffer(&Error);

        if (SUCCEEDED(HRes) && Error)
        {
            throw std::runtime_error(
                std::string("Compilation failed : ") + static_cast<const char*>(Error->GetBufferPointer()));
        }
    }

    // get spirv
    CComPtr<IDxcBlob> ShaderByteCode;
    CompileResult->GetResult(&ShaderByteCode);

    const auto pContent = static_cast<std::byte*>(ShaderByteCode->GetBufferPointer());
    const auto Size     = ShaderByteCode->GetBufferSize();

    return {pContent, pContent + Size};
}
} // namespace VT
