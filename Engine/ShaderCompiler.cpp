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

namespace VT::GLSL
{
/* =========================================================
 *                      GLSL_Compiler
 * =========================================================
 */
std::vector<uint32_t> Compiler::CompileSpv(ShaderFileInfo FileInfo)
{
    std::filesystem::path Src {FileInfo.FileDir};
    Src /= FileInfo.FileName;
    const auto File = File::ReadFile(Src);
    std::string Source {File.cbegin(), File.cend()};

    const shaderc::SpvCompilationResult Module {
        m_Compiler.CompileGlslToSpv(Source, FileInfo.ShaderType, FileInfo.FileName, FileInfo.CompileOptions)};

    if (Module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        VT_HALT("Failed to compile {0}, Message : {1}\n ", Src.string(), Module.GetErrorMessage());
    }

    return {Module.cbegin(), Module.cend()};
}
} // namespace VT::GLSL

namespace VT::HLSL
{
/* =========================================================
 *                      DXC_Compiler
 * =========================================================
 */
Compiler::Compiler()
{
    // Initialize DXC library
    VT_CORE_ASSERT(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&m_Library))),
                   "Could not init DXC Library\n");
    // Initialize DXC compiler
    VT_CORE_ASSERT(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_Compiler))),
                   "Could not init DXC Compiler\n");
    // Initialize DXC utility
    VT_CORE_ASSERT(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_DXC_Utils))),
                   "Could not init DXC Utiliy\n");
}

// must receive a CComPtr<IDxcBlob>, or dangling pointer
std::vector<std::byte> Compiler::CompileSpv(const ShaderFileInfo& File) const
{
    // parse path
    std::filesystem::path Src {File.FileDir};
    Src /= File.FileName;

    // Tokenize string to list of cli arguments
    std::vector<LPCWSTR> Args;
    Args.reserve(100);
    {
        /**
         * Finds how many characters until delim
         * StrStart : Starting position of string
         * Must be null terminating string
         */
        const auto FindDelim = [](LPCWSTR StrStart, WCHAR Delim) -> LPCWSTR
        {
            LPCWSTR pStr = StrStart;
            while (*pStr)
            {
                if (*pStr == Delim)
                {
                    return pStr;
                }
                pStr++;
            }

            return pStr;
        };

        const auto StrCpy = [](LPWSTR Str, uint32_t Count, LPCWSTR Src)
        {
            int i = 0;
            for (; i < Count && Src[i] != L'\0'; i++)
            {
                Str[i] = Src[i];
            }
            Str[i] = L'\0';
        };

        LPCWSTR Ptr = File.pCL_Args;
        LPCWSTR Pos = FindDelim(Ptr, L' ');
        while (*Pos)
        {
            const size_t Amount = Pos - Ptr;
            LPWSTR Str          = new WCHAR[Amount + 1];
            StrCpy(Str, Amount, Ptr);
            Args.push_back(Str);

            Ptr = Pos + 1;
            Pos = FindDelim(Ptr, L' ');
        }

        // Copy last segment of string
        const size_t Amount = Pos - Ptr;
        LPWSTR Str          = new WCHAR[Amount + 1];
        StrCpy(Str, Amount, Ptr);
        Args.push_back(Str);

        // Append file name
        Args.push_back((LPCWSTR) Src.wstring().c_str());
    }

    UINT32 Encoding = File.Encoding;

    // load file
    CComPtr<IDxcBlobEncoding> SourceBlob;
    HRESULT HRes = m_DXC_Utils->LoadFile(Src.wstring().c_str(), &Encoding, &SourceBlob);
    VT_CORE_ASSERT(SUCCEEDED(HRes), "Could not load shader file : {0}", Src.string());

    DxcBuffer SrcBuff {
        .Ptr = SourceBlob->GetBufferPointer(), .Size = SourceBlob->GetBufferSize(), .Encoding = Encoding};

    // compile
    CComPtr<IDxcResult> CompileResult;
    HRes = m_Compiler->Compile(
        &SrcBuff, Args.data(), static_cast<uint32_t>(Args.size()), nullptr, IID_PPV_ARGS(&CompileResult));
    // Free all string except last : Last string is owned by std::filesystem::path Src
    for (int i = 0; i < Args.size() - 1; i++)
    {
        delete[] Args[i];
    }

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
            VT_CORE_ERROR("HLSL Shader Compilation failed : {}", static_cast<const char*>(Error->GetBufferPointer()));
        }
    }

    // get spirv
    CComPtr<IDxcBlob> ShaderByteCode;
    CompileResult->GetResult(&ShaderByteCode);

    const auto pContent = static_cast<std::byte*>(ShaderByteCode->GetBufferPointer());
    const auto Size     = ShaderByteCode->GetBufferSize();

    return {pContent, pContent + Size};
}
} // namespace VT::HLSL
