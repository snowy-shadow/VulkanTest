#pragma once

#include "Instance.h"
#include "Device.h"
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
		Device m_VulkanDevice;
	
		vk::CommandPool m_CmdPool;
	
		Window m_Window{ {1280, 720}, "VulkanTest" };
	};
}
