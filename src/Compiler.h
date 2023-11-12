#pragma once

#include "File.h"

#ifdef _WIN32
/*
 * CComPtr
 * MUST COME BEFORE dxcapi.h && BEFORE vulkan_hash.hpp
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

	class DXC_Compiler
	{
	public:
		DXC_Compiler();
		CComPtr<IDxcBlob> compile(ShaderFileInfo) const;

		DXC_Compiler(const DXC_Compiler&) = delete;
		DXC_Compiler& operator = (const DXC_Compiler&) = delete;
	private:
		CComPtr<IDxcCompiler3> m_DXC_Compiler;
		CComPtr<IDxcUtils> m_DXC_Utils;
	};

	class GLSL_Compiler
	{
	public:
		GLSL_Compiler();
		std::vector<uint32_t> compile(ShaderFileInfo);

		GLSL_Compiler(const GLSL_Compiler&) = delete;
		GLSL_Compiler& operator = (const GLSL_Compiler&) = delete;
	private:

	};
}