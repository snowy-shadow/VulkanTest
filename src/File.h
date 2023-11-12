#pragma once

// conversion from wchar_t to _Elem, possible loss of data (wchar -> char)
#pragma warning(disable : 4244)

#include <string>
#include <vector>
#include <fstream>

namespace VT
{
	struct FileInfo
	{
		std::wstring FileLocation;
		std::wstring FileName;
		uint32_t Encoding;
	};

	struct ShaderFileInfo : public FileInfo
	{
		std::vector<const wchar_t*> CL_Args;
	};

	std::vector<std::byte> inline readFile(const std::wstring& FilePath)
	{
		std::ifstream File(FilePath, std::ios::binary | std::ios::ate);

		if (!File.is_open()) throw std::runtime_error("Failed to open : " + std::string(FilePath.cbegin(), FilePath.cend()));

		const auto FileSize = static_cast<std::streamsize>(File.tellg());
		std::vector<std::byte> Buffer(FileSize);

		File.seekg(0);
		File.read((char*)Buffer.data(), FileSize);

		File.close();
		return { Buffer.begin(), Buffer.end() };
	}
}