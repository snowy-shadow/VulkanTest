#include "App.h"

// enable aggregate initialization
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_structs.hpp>

namespace VT
{
	App::App()
	{
		uint32_t glfwExtensionCount;
		const char** GLFW_Extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		m_VulkanInstance.initInstance( {.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0) }, { GLFW_Extensions, GLFW_Extensions + glfwExtensionCount }, {});

		m_VulkanDevice.bindInstance(m_VulkanInstance.getInstance());
		m_VulkanDevice.createDevice("Main",
			m_Window.m_Window, 
			{ "VK_KHR_swapchain" }, 
			{ {vk::QueueFlagBits::eGraphics, 1.f}, {vk::QueueFlagBits::eTransfer, 1.f} });

		m_Renderer.bindDevices(m_VulkanDevice.getDeviceReferences("Main"), &m_Window);

		m_Renderer.init();


	}

	void App::run()
	{
		while (!glfwWindowShouldClose(m_Window.m_Window))
		{
			glfwPollEvents();
			m_Renderer.update();
		}
	}


}
