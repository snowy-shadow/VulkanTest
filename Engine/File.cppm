module;
#include "VT_Export"

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

export module VT.File;

export namespace VT::File
{
struct VT_ENGINE_EXPORT FileInfo
{
    const char* FileDir;
    const char* FileName;
};

[[nodiscard]]
std::vector<char> ReadFile(const std::filesystem::path& FilePath)
{
    std::ifstream File(FilePath, std::ios::binary | std::ios::in | std::ios::ate);

    if (!File.is_open())
        throw std::runtime_error("Failed to open : " + FilePath.string());

    auto FileSize = static_cast<const unsigned int>(File.tellg());
    std::vector<char> Buffer(FileSize);

    File.seekg(0);
    File.read(Buffer.data(), FileSize);

    File.close();
    return Buffer;
};
} // namespace VT::File

namespace std
{
template <>
struct hash<VT::File::FileInfo>
{
    size_t operator()(const VT::File::FileInfo& F) const noexcept
    {
        return hash<std::string>()(F.FileName) ^ (std::filesystem::hash_value(F.FileDir) >> 3) ^ 11 << 7;
    }
};
} // namespace std
