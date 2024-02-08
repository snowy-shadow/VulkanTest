#include "ShaderCompiler.h"
#include "ShaderCompiler.h"

namespace VT
{
	void ShaderCompiler::setWorkingDir(std::filesystem::path DestFolderPath)
	{
		namespace fs = std::filesystem;
        m_WorkingDir = DestFolderPath;
        if(fs::exists(DestFolderPath))
        {
            // file that contains edit time stamps
            m_ShaderLog = File::readFileDelim(m_WorkingDir / "spv_log.txt", ';', ':');
            return;
        }

        // create it
        if (!fs::create_directory(DestFolderPath)) { throw std::runtime_error("Failed to create folder : " + DestFolderPath.string() + "!\n"); }
    }

	std::vector<std::vector<uint32_t>> ShaderCompiler::compileShaders(const std::vector<File::DXC_ShaderFileInfo>& ShaderInfos) const
	{
		std::vector<std::vector<uint32_t>> ShaderSpv;

		for(auto& F : ShaderInfos)
		{
			ShaderSpv.emplace_back(compileShader(F));
		}

		return ShaderSpv;
	}

    /*
     * create a binary search tree, add file names as nodes <- using std::unordered set
     * ...FilePathFileName:Timestamp;...
     * multithreaded compilation where they share the binary tree, avoid data race with lock
     * this avoids copying in case of vectors
     */


	std::vector<uint32_t> ShaderCompiler::compileShader(File::DXC_ShaderFileInfo FileInfo) const
	{
		return fileToSpv(std::forward<const File::DXC_ShaderFileInfo>(FileInfo));
	}


	/*
	* ==================================================
	*					    PRIVATE
	* ==================================================
	*/

	//std::vector<std::byte> Pipeline::compileFile(DXC_ShaderFileInfo FileInfo)
	//{
	//	namespace fs = std::filesystem;

	//	// src file
	//	const fs::path SrcPath(FileInfo.FileLocation + FileInfo.FileName);

	//	// spv binary
	//	const auto OutPath{ (m_WorkingDir / FileInfo.FileName).replace_extension("spv") };
	//	const bool bFileExist{ fs::exists(OutPath) };

	//	const auto CurrentTime{ std::filesystem::last_write_time(SrcPath) };

	//	if (bFileExist &&  CurrentTime > getFileLastEditTime(SrcPath))
	//	{
	//		replaceLine(m_ShaderLog, SrcPath.string(), std::format("{%F%T}={}\n", OutPath, CurrentTime));
	//		return readFile(OutPath);
	//	}

	//	auto Spirv = forceCompileFile(FileInfo);

	//	overwriteFile(OutPath, Spirv);

	//	auto s = std::format("{%F%T}={}\n", SrcPath, CurrentTime);
	//	appendToFile(m_ShaderLog, { s.cbegin(), s.cend() });

	//	return Spirv;

	//}

	std::vector<uint32_t> ShaderCompiler::fileToSpv(File::DXC_ShaderFileInfo FileInfo) const
	{
        DXC_Compiler m_Compiler;
        const CComPtr<IDxcBlob> ResPtr = m_Compiler.compile(FileInfo);

		const auto pContent = static_cast<char*>(ResPtr->GetBufferPointer());
		const auto Size = ResPtr->GetBufferSize();

		return { pContent, pContent + Size };
	}

}