#pragma once

#include "File.h"

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.hpp>

namespace VT
{
    struct ShadercFileInfo : public FileInfo
    {
        vk::ShaderStageFlagBits Stage;
        shaderc_shader_kind ShaderType;
        shaderc::CompileOptions CompileOptions;
    };

	class GLSL_Compiler
	{
	public:
		GLSL_Compiler() = default;

        [[nodiscard]]
		std::vector<uint32_t> compile(ShadercFileInfo);

		GLSL_Compiler(const GLSL_Compiler&) = delete;
		GLSL_Compiler& operator = (const GLSL_Compiler&) = delete;
        ~GLSL_Compiler() = default;
	private:
        shaderc::Compiler m_Compiler;
	};
}

namespace std
{
    template<>
    struct hash<VT::ShadercFileInfo>
    {
        size_t operator()(const VT::ShadercFileInfo& F) const noexcept
        {
            // can't hash shaderc::CompileOptions
            return hash<shaderc_shader_kind>()(F.ShaderType) ^ (hash<VT::FileInfo>()(F) << 7);
        }
    };
}
