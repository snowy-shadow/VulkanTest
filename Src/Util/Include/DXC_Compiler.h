#pragma once

#include "File.h"

#ifdef _WIN32
/*
 * CComPtr
 * MUST COME BEFORE dxcapi.h && AFTER vulkan_hash.hpp
*/
#include <atlcomcli.h>
#endif

// provides cross platform CComPtr if not WIN32
#include <dxc/dxcapi.h>
#include <vulkan/vulkan.hpp>

namespace VT
{
	namespace File
	{
		enum DXC_FileEncoding : uint32_t
		{
			eACP = DXC_CP_ACP,
			eUTF8 = DXC_CP_UTF8,
			eUTF16 = DXC_CP_UTF16,
			eUTF32 = DXC_CP_UTF32,
			eWide = DXC_CP_WIDE,
		};

		struct DXC_ShaderFileInfo : public FileInfo
		{
			vk::ShaderStageFlagBits Stage;
			std::vector<LPCWSTR> CL_Args;
		};
	}

	class DXC_Compiler
	{
	public:
		DXC_Compiler();

		[[nodiscard]]
		CComPtr<IDxcBlob> compile(File::DXC_ShaderFileInfo&) const;

		DXC_Compiler(const DXC_Compiler&) = delete;
		DXC_Compiler& operator = (const DXC_Compiler&) = delete;
		~DXC_Compiler() = default;
	private:
		CComPtr<IDxcCompiler3> m_DXC_Compiler = nullptr;
		CComPtr<IDxcUtils> m_DXC_Utils = nullptr;
	};
}

namespace std
{
	template<>
	struct hash<VT::File::DXC_ShaderFileInfo>
	{
		size_t operator()(const VT::File::DXC_ShaderFileInfo& F) const noexcept
		{
			return hash<LPCWSTR const*>()(F.CL_Args.data()) ^ (hash<VT::File::FileInfo>()(F) << 7);
		}
	};
}