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
		void createMainSwapchain();
		void createMainGraphicPipeline();

		Instance m_VulkanInstance;
		Renderer m_Renderer;
		vk::CommandPool m_CmdPool;
		vk::Device m_LogicDevice;
		PhysicalDevice const* m_PhysicalDevice;
	
		Window m_Window{ {1280, 720}, "VulkanTest" };
	};
}
