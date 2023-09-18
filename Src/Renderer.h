#pragma once

#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"


namespace VT
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		GLFWwindow* getWindow();
		void createSwapChain();

		void update();
	
	private:
		Window m_Window{ {1280, 720}, "VulkanTest" };
		Pipeline GraphicsPipeLine{ "../Shaders/Vertex.spv", "../Shaders/Fragment.spv" };
		std::vector<SwapChain> m_SwapChain;
	};
}
