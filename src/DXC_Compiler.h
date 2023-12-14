#pragma once

#include "File.h"

#ifdef _WIN32
/*
 * CComPtr
 * MUST COME BEFORE dxcapi.h && AFTER vulkan_hash.hpp
*/
#include <atlcomcli.h>
#endif

// provides crossplatform CComPtr if not WIN32
#include <dxc/dxcapi.h>

namespace VT
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
		std::wstring CL_Args;
	};

	class DXC_Compiler
	{
	public:
		DXC_Compiler();

		[[nodiscard]]
		CComPtr<IDxcBlob> compile(DXC_ShaderFileInfo) const;

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
	struct hash<VT::DXC_ShaderFileInfo>
	{
		size_t operator()(const VT::DXC_ShaderFileInfo& F) const noexcept
		{
			return hash<std::wstring>()(F.CL_Args) ^ (hash<VT::FileInfo>()(F) << 7);
		}
	};
}