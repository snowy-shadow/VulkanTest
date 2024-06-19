module;
#include "VT_Export"

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

export module VT.ShaderCompiler;

import VT.File;

export namespace VT
{

/* =========================================================
 *                      GLSL_Compiler
 * =========================================================
 */
struct GLSL_FileInfo : File::FileInfo
{
    vk::ShaderStageFlagBits Stage;
    shaderc_shader_kind ShaderType;
    shaderc::CompileOptions CompileOptions;
};

class VT_ENGINE_EXPORT GLSL_Compiler
{
public:
    GLSL_Compiler() = default;

    [[nodiscard]]
    std::vector<uint32_t> CompileSpv(File::ShadercFileInfo);

    GLSL_Compiler(const GLSL_Compiler&)            = delete;
    GLSL_Compiler& operator=(const GLSL_Compiler&) = delete;

    ~GLSL_Compiler() = default;

private:
    shaderc::Compiler m_Compiler;
};
};

/* =========================================================
 *                      DXC_Compiler
 * =========================================================
 */
enum DXC_FileEncoding : uint32_t
{
    DXC_FileEncodingACP   = DXC_CP_ACP,
    DXC_FileEncodingUTF8  = DXC_CP_UTF8,
    DXC_FileEncodingUTF16 = DXC_CP_UTF16,
    DXC_FileEncodingUTF32 = DXC_CP_UTF32,
    DXC_FileEncodingWide  = DXC_CP_WIDE,
};

struct VT_ENGINE_EXPORT DXC_ShaderFileInfo : File::FileInfo
{
    uint32_t Encoding;
    vk::ShaderStageFlagBits Stage;
    std::vector<LPCWSTR> CL_Args;
};

class VT_ENGINE_EXPORT DXC_Compiler
{
public:
    DXC_Compiler();

    [[nodiscard]]
    std::vector<std::byte> CompileSpv(DXC_ShaderFileInfo&) const;

    DXC_Compiler(const DXC_Compiler&)            = delete;
    DXC_Compiler& operator=(const DXC_Compiler&) = delete;
    ~DXC_Compiler()                              = default;

private:
    CComPtr<IDxcCompiler3> m_DXC_Compiler = nullptr;
    CComPtr<IDxcUtils> m_DXC_Utils        = nullptr;
};
} // namespace VT
