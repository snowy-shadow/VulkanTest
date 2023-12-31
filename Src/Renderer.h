#pragma once

#include "SwapChain.h"
/*
 * Must be included after all instances, of #include <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/
//#include "ShaderCompiler.h"

#include "Window.h"
#include "Instance.h"

#include <list>
#include <unordered_map>

namespace VT
{
	class Renderer
	{
	public:
		void createSwapChain(const std::unordered_set<std::string_view>& SwapChainName);
		void bindInstance(Instance& Instance);
		void bindWindow(Window& Window);

//		void addShaderStage(const std::vector<DXC_ShaderFileInfo>&);

        vk::Pipeline createGraphicsPipeline();

		void update();
		void destroy();

		Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;
	private:
		Window* m_Window{nullptr};
//		ShaderCompiler m_ShaderCompiler;

		std::list<SwapChain> m_SwapChain;
		std::unordered_map<std::string_view, uint32_t> m_SwapChainReference;

        vk::Pipeline m_Pipeline;

		// Instance handle
		Instance* m_Instance{ nullptr };
	};
}
