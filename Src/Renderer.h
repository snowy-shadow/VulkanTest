#pragma once

#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"

namespace VT
{
	class Instance;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void createSwapChain(const uint32_t& Amount = 1);
		void bindInstance(Instance& Instance);
		void bindWindow(Window& Window);

		void update();
	
	private:
		Window* m_Window;
		Pipeline GraphicsPipeLine{ "../Shaders/Vertex.spv", "../Shaders/Fragment.spv" };
		std::vector<SwapChain> m_SwapChain;

		// Instance handle
		Instance* m_Instance{ nullptr };
	};
}
