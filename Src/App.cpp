#include "App.h"


namespace VT
{
	App::App()
	{
		m_VulkanInstance.initInstance({ .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) });
		m_VulkanInstance.initDevice(m_Window.m_Window, { "VK_KHR_swapchain" }, { {vk::QueueFlagBits::eGraphics, 1.f} });

		m_Renderer.bindInstance(m_VulkanInstance);
		m_Renderer.bindWindow(m_Window);
		m_Renderer.createSwapChain({"Main"});

	}

	void VT::App::run()
	{
		// maybe put this into VT::Window
		while (!glfwWindowShouldClose(m_Window.m_Window))
		{
			glfwPollEvents();
		}
	}

	App::~App()
	{
		m_Renderer.destroy();
		m_VulkanInstance.destroy();
	}



	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */
}
