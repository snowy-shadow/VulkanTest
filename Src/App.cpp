#include "App.h"


namespace VT
{
	App::App()
	{
		m_VulkanInstance.initInstance({ .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) });
		m_VulkanInstance.initDevice(m_Renderer.getWindow(), { "VK_KHR_swapchain" });
		m_Renderer.createSwapChain(m_VulkanInstance.getPhysicalDevice());
	}



	void VT::App::run()
	{
		// maybe put this into VT::Window
		while (!glfwWindowShouldClose(m_Renderer.getWindow()))
		{
			glfwPollEvents();
		}
	}

	App::~App()
	{
	}



	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	
}
