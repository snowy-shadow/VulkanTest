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

		// Renderer depends on Instance, Renderer must be destroyed first
		Instance m_VulkanInstance;
		Renderer m_Renderer;

		Window m_Window{ {1280, 720}, "VulkanTest" };
	};
}