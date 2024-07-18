module;
#include "VT_Export"
#include <shaderc/shaderc.hpp>

#include <vulkan/vulkan.hpp>

#ifdef _WIN32
    /*
     * CComPtr
     * MUST COME BEFORE dxcapi.h && AFTER vulkan_hash.hpp
     */
    #include <atlcomcli.h>
#endif

// provides cross platform CComPtr if not WIN32
#include <dxc/dxcapi.h>


export module VT.ShaderCompiler;

import VT.File;

export namespace VT::GLSL
{

/* =========================================================
 *                      GLSL_Compiler
 * =========================================================
 */
struct VT_ENGINE_EXPORT ShaderFileInfo : File::FileInfo
{
    vk::ShaderStageFlagBits Stage;
    shaderc_shader_kind ShaderType;
    shaderc::CompileOptions CompileOptions;
};

class VT_ENGINE_EXPORT Compiler
{
public:
    Compiler() = default;

    [[nodiscard]] std::vector<uint32_t> CompileSpv(ShaderFileInfo);

    Compiler(const Compiler&)            = delete;
    Compiler& operator=(const Compiler&) = delete;

    ~Compiler() = default;

private:
    shaderc::Compiler m_Compiler;
};
} // namespace VT::GLSL

export namespace VT::HLSL
{
/* =========================================================
 *                      DXC_Compiler
 * =========================================================
 */
enum FileEncoding : uint32_t
{
    DXC_FileEncodingACP   = DXC_CP_ACP,
    DXC_FileEncodingUTF8  = DXC_CP_UTF8,
    DXC_FileEncodingUTF16 = DXC_CP_UTF16,
    DXC_FileEncodingUTF32 = DXC_CP_UTF32,
    DXC_FileEncodingWide  = DXC_CP_WIDE,
};

struct VT_ENGINE_EXPORT ShaderFileInfo : File::FileInfo
{
    LPCWSTR pCL_Args;
    uint32_t CL_ArgSize;
    vk::ShaderStageFlagBits Stage;
    uint32_t Encoding = DXC_FileEncodingUTF8;
};

class VT_ENGINE_EXPORT Compiler
{
public:
    Compiler();

    [[nodiscard]] std::vector<std::byte> CompileSpv(const ShaderFileInfo&) const;

    Compiler(const Compiler&)                    = delete;
    Compiler& operator=(const Compiler&) = delete;
    ~Compiler()                                  = default;

private:
    CComPtr<IDxcCompiler3> m_Compiler = nullptr;
    CComPtr<IDxcUtils> m_DXC_Utils        = nullptr;
};
}
