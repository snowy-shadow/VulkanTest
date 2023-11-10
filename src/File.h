#pragma once

#include <string>
#include <vector>
#include <fstream>

// ========================= Copied from <dxc/dxcapi.h> =========================

// For convenience, equivalent definitions to CP_UTF8 and CP_UTF16.
#define DXC_CP_UTF8 65001
#define DXC_CP_UTF16 1200
#define DXC_CP_UTF32 12000
// Use DXC_CP_ACP for: Binary;  ANSI Text;  Autodetect UTF with BOM
#define DXC_CP_ACP 0

/// Codepage for "wide" characters - UTF16 on Windows, UTF32 on other platforms.
#ifdef _WIN32
#define DXC_CP_WIDE DXC_CP_UTF16
#else
#define DXC_CP_WIDE DXC_CP_UTF32
#endif

// ==============================================================================

enum FileEncoding : uint32_t
{
	eACP = DXC_CP_ACP,
	eUTF8 = DXC_CP_UTF8,
	eUTF16 = DXC_CP_UTF16,
	eUTF32 = DXC_CP_UTF32,
	eWide = DXC_CP_WIDE,
};

struct FileInfo
{
	std::wstring FileLocation;
	std::wstring FileName;
	FileEncoding Encoding;
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