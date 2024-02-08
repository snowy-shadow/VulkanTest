//
// Created by Kuang on 2023-12-26.
//

#include "GLSL_Compiler.h"

namespace VT
{
    std::vector<uint32_t> GLSL_Compiler::compile(File::ShadercFileInfo FileInfo)
    {
        auto File = File::readFile(FileInfo.FileLocation + FileInfo.FileName);
        std::string Source { File.cbegin(), File.cend() };

        shaderc::SpvCompilationResult Module
        {
            m_Compiler.CompileGlslToSpv(
                Source,
                FileInfo.ShaderType,
                FileInfo.FileName.c_str(),
                FileInfo.CompileOptions)
        };

        if(Module.GetCompilationStatus() != shaderc_compilation_status_success)
        { throw std::runtime_error("Failed to compile " + FileInfo.FileLocation + "/" + FileInfo.FileName + " Message : \n" + Module.GetErrorMessage()); }

        return { Module.cbegin(), Module.cend() };
    }

}
