#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace VT
{
	struct FileInfo
	{
		std::wstring FileLocation;
		std::wstring FileName;
		uint32_t Encoding;
	};

	[[nodiscard]]
	std::vector<std::byte> inline readFile(const std::filesystem::path& FilePath)
	{
		std::ifstream File(FilePath, std::ios::binary | std::ios::in | std::ios::ate);

		if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath.string());

		const auto FileSize = static_cast<std::streamsize>(File.tellg());
		std::vector<std::byte> Buffer(FileSize);

		File.seekg(0);
		File.read((char*)Buffer.data(), FileSize);

		File.close();
		return { Buffer.begin(), Buffer.end() };
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

	std::filesystem::file_time_type inline getFileLastEditTime(const std::filesystem::path& FilePath)
	{
		std::ifstream File(FilePath, std::ios::in);
		if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath.string());

		const auto PathString = FilePath.string();

		std::string Line;
		bool bFound{false};

		while (std::getline(File, Line))
		{
			if(Line.find(PathString))
			{
				bFound = true;
				break;
			}
		}
		File.close();

		if(!bFound)
		{
			return std::filesystem::file_time_type::min();
		}

		// check!
		const auto Begin = FilePath.c_str() + PathString.size();
		const auto End = Begin + (Line.length() - PathString.size());

		std::chrono::file_time FileTime;
		std::wstringstream ss{ std::wstring{ Begin, End } };
		return std::chrono::from_stream(ss, "%F%T", FileTime);
	};

	void inline replaceLine(const std::filesystem::path& FilePath, const std::string_view Find, const std::string_view Replacement)
	{
		std::fstream File(FilePath, std::ios::out | std::ios::in);
		if (!File.is_open()) throw std::runtime_error("Failed to open : " + FilePath.string());

		std::string Line;
		while (std::getline(File, Line))
		{
			if (Line.find(Find) != std::string::npos)
			{
				if (Line.length() != Replacement.size()) { throw std::runtime_error("Mismatch length, Replacing " + Line + " with " + Replacement.data()); }

				// check!
				File.write(Replacement.data(), static_cast<std::streamsize>(Replacement.size()));
			}
		}

		File.close();
	}
}


namespace std
{
	template<>
	struct hash<VT::FileInfo>
	{
		size_t operator()(const VT::FileInfo& F) const noexcept
		{
			return hash<std::wstring>()(F.FileName) ^ ((hash<std::wstring>()(F.FileLocation) >> 2) ^ (hash<uint32_t>()(F.Encoding) << 5) << 7);
		}
	};
}