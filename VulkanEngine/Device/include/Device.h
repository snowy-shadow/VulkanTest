#pragma once

#include "PhysicalDevice.h"

#include <unordered_map>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VT
{
	class Device
	{
	public:
		
		void bindInstance(vk::Instance VulkanInstance);
		/*
		 * Creates Physical and Logical device with given Name
		 * Name - PhysicalDevice Name, LogicalDevice Name
		 * Window - glfw window handle
		 * RequiredExtensions - Physical device extensions
		 * RequiredQueues - Physical device queues
		 */
		void createDevice(std::string Name, GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::pair<vk::QueueFlagBits, float>>& RequiredQueues);

		/*
		 * Device - LogicalDevice Name
		*/
		std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> getDeviceReferences(std::string Device);

		~Device();

	private:
		vk::SurfaceKHR m_Surface;

		PhysicalDevice m_PhysicalDevice;
		std::unordered_map<std::string, vk::Device> m_LogicalDevices;

		enum ObjectConstructed : uint8_t
		{
			eSurface = 0b1
		};
		uint8_t m_ObjectConstructedMask{0};
		// Handles
		vk::Instance m_VulkanInstance;
		
	};
}

