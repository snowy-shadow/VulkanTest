//
// Created by Kuang on 2023-12-26.
//

#include "GLSL_Compiler.h"

namespace VT
{
    std::vector<uint32_t> GLSL_Compiler::compile(File::ShadercFileInfo FileInfo)
    {
        const auto Src{ FileInfo.FileLocation / FileInfo.FileName };
        const auto File = File::readFile(Src);
        std::string Source { File.cbegin(), File.cend() };

        const shaderc::SpvCompilationResult Module
        {
            m_Compiler.CompileGlslToSpv(
                Source,
                FileInfo.ShaderType,
                FileInfo.FileName.c_str(),
                FileInfo.CompileOptions)
        };

        if(Module.GetCompilationStatus() != shaderc_compilation_status_success)
        { throw std::runtime_error("Failed to compile " + Src.string() + " Message : \n" + Module.GetErrorMessage()); }

        return { Module.cbegin(), Module.cend() };
    }

}
