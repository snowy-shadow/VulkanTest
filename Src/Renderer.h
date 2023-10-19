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
		Renderer();
		~Renderer();

		void createSwapChain(const uint32_t& Amount = 1);
		void bindInstance(Instance& Instance);
		void bindWindow(Window& Window);

		void update();
		void destroy();
	
	private:
		Window* m_Window;
		Pipeline GraphicsPipeLine{ "../Shaders/Vertex.spv", "../Shaders/Fragment.spv" };
		std::list<SwapChain> m_SwapChain;

		// Instance handle
		Instance* m_Instance{ nullptr };
	};
}
