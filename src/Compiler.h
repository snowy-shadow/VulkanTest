#pragma once

#include "File.h"

#ifdef _WIN32
// CComPtr
#include <atlcomcli.h>
#endif
// provides crossplatform CComPtr if not WIN32
#include <dxc/dxcapi.h>

class DXC_Compiler
{
public:
	DXC_Compiler();
	CComPtr<IDxcBlob> compile(ShaderFileInfo);

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