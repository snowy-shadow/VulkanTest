#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace VT::File
{
	struct FileInfo
	{
		std::filesystem::path FileLocation;
		std::string FileName;
		uint32_t Encoding;
	};

	[[nodiscard]]
	std::vector<char> inline readFile(const std::filesystem::path& FilePath)
	{
		std::ifstream File(FilePath, std::ios::binary | std::ios::in | std::ios::ate);

		if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath.string());

		auto FileSize = static_cast<const unsigned int>(File.tellg());
		std::vector<char> Buffer(FileSize);

		File.seekg(0);
		File.read(Buffer.data(), FileSize);

		File.close();
		return Buffer;
	}

    // returns strings by delim(not included)
    // if no delim, return vector with 1 string
    [[nodiscard]]
    std::unordered_map<std::string, std::string> inline readFileDelim(
            const std::filesystem::path& FilePath,
            char FileDelim,
            char TimeDelim)
    {
        std::ifstream File(FilePath, std::ios::in);

        if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath.string());

        std::unordered_map<std::string, std::string> Buffer{};

        std::string S;
        while(std::getline(File, S, FileDelim))
        {
            auto Index = S.find(TimeDelim);
            Buffer[S.substr(0, Index)] = S.substr(Index + 1);
        }

        File.close();
        return Buffer;
    }

	// creates and writes to file
	void inline overwriteFile(const std::filesystem::path& FilePath, const std::vector<std::byte>& Content)
	{
		std::ofstream File(FilePath, std::ios::binary | std::ios::out | std::ios::trunc);

		if (!File.is_open()) { throw std::runtime_error("Failed to open : " + FilePath.string()); }

		File.write(reinterpret_cast<const char*>(Content.data()), static_cast<std::streamsize>(Content.size()));
		File.close();
	}

	// creates and writes to file
	void inline appendToFile(const std::filesystem::path& FilePath, const std::vector<std::byte>& Content)
	{
		std::ofstream File(FilePath, std::ios::binary | std::ios::out | std::ios::app);

		if (!File.is_open()) { throw std::runtime_error("Failed to open : " + FilePath.string()); }

		File.write(reinterpret_cast<const char*>(Content.data()), static_cast<std::streamsize>(Content.size()));
		File.close();

	}

	//std::filesystem::time_point inline getFileLastEditTime(const std::filesystem::path& FilePath)
	//{
	//	std::ifstream File(FilePath, std::ios::in);
	//	if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath.string());

	//	const auto PathString = FilePath.string();

	//	std::string Line;
	//	bool bFound{false};

	//	while (std::getline(File, Line))
	//	{
	//		if(Line.find(PathString))
	//		{
	//			bFound = true;
	//			break;
	//		}
	//	}
	//	File.close();

	//	if(!bFound)
	//	{
	//		return std::filesystem::file_time_type::min();
	//	}

	//	// check!
	//	const auto Begin = FilePath.c_str() + PathString.size();
	//	const auto End = Begin + (Line.length() - PathString.size());

	//	std::chrono::time_point<std::chrono::system_clock> FileTime;
	//	std::stringstream ss{ std::string{ Begin, End } };
	//	return std::chrono::from_stream(ss, "%F%T", FileTime);
	//};
}

namespace std
{
	template<>
	struct hash<VT::File::FileInfo>
	{
		size_t operator()(const VT::File::FileInfo& F) const noexcept
		{
			return hash<std::string>()(F.FileName) ^ (std::filesystem::hash_value(F.FileLocation) >> 2) ^ (hash<uint32_t>()(F.Encoding) << 5) << 7;
		}
	};
}
