#pragma once

#include "PhysicalDevice.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <unordered_map>
#include <tuple>
#include <GLFW/glfw3.h>

namespace VT
{
	class Instance
	{
	public:
		/*
		 * Create vulkan instance
		 */
		void initInstance(const vk::ApplicationInfo& ApplicationInfo, std::vector<const char*> Required_GLFW_Extensions);

		/*
		 * Creates Physical and Logical device with given Name
		 * Name - PhysicalDevice Name, LogicalDevice Name
		 * Window - glfw window handle
		 * RequiredExtensions - Physical device extensions
		 * RequiredQueues - Physical device queues
		 */
		void createDevice(std::array<std::string, 2> Name, GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues);

		/*
		 * Device - PhysicalDevice Name, LogicalDevice Name
		*/
		std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> getDeviceReferences(std::array<std::string, 2> Device);

		Instance() = default;
		~Instance();
		Instance(Instance&) = delete;
		Instance& operator=(Instance&) = delete;

	private:
		bool isSupported(std::vector<const char*> RequiredExtensions = {}, std::vector<const char*> RequiredLayers = {}) const;

		vk::Instance m_VulkanInstance;
		vk::SurfaceKHR m_Surface;

		std::unordered_map<std::string, PhysicalDevice> m_PhysicalDevices{};
		std::unordered_map<std::string, vk::Device> m_LogicalDevices;
		uint8_t m_ObjectConstructed{0};

#ifndef NDEBUG
		vk::DispatchLoaderDynamic m_DLD_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}
