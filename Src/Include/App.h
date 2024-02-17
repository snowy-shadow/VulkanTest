#pragma once

#include "Instance.h"
#include "Renderer.h"
#include "Window.h"

namespace VT
{
	class App
	{
	public:
		App();
		void run();

	private:

		void createMainGraphicPipeline();

		Window m_Window{ {1280, 720}, "VulkanTest" };
		// Renderer depends on Instance, Renderer must be destroyed first

		Instance m_VulkanInstance{ {.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) } };
		Renderer m_Renderer;
	

	};
}