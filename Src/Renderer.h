#pragma once
#include <list>

#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"
#include "Instance.h"

namespace VT
{
	class Renderer
	{
	public:
		void createSwapChain(const uint32_t& Amount = 1);
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


		// Instance handle
		Instance* m_Instance{ nullptr };
	};
}
