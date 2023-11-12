#pragma once

#include "SwapChain.h"
/*
 * Must be included after all instances, of #include <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/
#include "Pipeline.h"

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

		void update();
		void destroy();

		Renderer() {};
		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;
	private:
		Window* m_Window{nullptr};
		Pipeline GraphicsPipeLine{ "../Shaders/Vertex.spv", "../Shaders/Fragment.spv" };

		std::list<SwapChain> m_SwapChain;
		std::unordered_map<std::string_view, uint32_t> m_SwapChainReference;

		// Instance handle
		Instance* m_Instance{ nullptr };
	};
}
