#include "Pipeline.h"

namespace VT
{
	void Pipeline::setWorkingDir(std::filesystem::path DestFolderPath)
	{
		namespace fs = std::filesystem;
        m_WorkingDir = DestFolderPath;
        if(fs::exists(DestFolderPath))
        {
            // file that contains edit time stamps
            m_ShaderLog = readFileDelim(m_WorkingDir / "spv_log.txt", ';', ':');
            return;
        }

        // create it
        if (!fs::create_directory(DestFolderPath)) { throw std::runtime_error("Failed to create folder : " + DestFolderPath.string() + "!\n"); }
    }

    /*
     * create a binary search tree, add file names as nodes <- using std::unordered set
     * ...FilePathFileName:Timestamp;...
     * multithreaded compilation where they share the binary tree, avoid data race with lock
     * this avoids copying in case of vectors
     */


	vk::ShaderModule Pipeline::createShaderModule(DXC_ShaderFileInfo FileInfo, vk::Device& Device) const
	{
		auto Spv = fileToSpv(std::forward<const DXC_ShaderFileInfo>(FileInfo));

		return Device.createShaderModule(
			{
				// in bytes
				.codeSize = Spv.size(),
				.pCode = reinterpret_cast<const uint32_t*>(Spv.data())
			}
		);
	}

	void Pipeline::appendShaderStage(vk::PipelineShaderStageCreateInfo Info)
	{
		m_ShaderStages.push_back(Info);
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

	std::vector<std::byte> Pipeline::fileToSpv(DXC_ShaderFileInfo FileInfo) const
	{
        DXC_Compiler m_Compiler;
        const CComPtr<IDxcBlob> ResPtr = m_Compiler.compile(FileInfo);

		const auto pContent = static_cast<std::byte*>(ResPtr->GetBufferPointer());
		const auto Size = ResPtr->GetBufferSize();

		return { pContent, pContent + Size };
	}

}