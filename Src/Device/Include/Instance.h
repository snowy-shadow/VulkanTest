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
		explicit Instance(const vk::ApplicationInfo& ApplicationInfo);
		void initInstance(const vk::ApplicationInfo& ApplicationInfo);

		void createDevice(std::string Name, GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues);

		std::tuple<std::unordered_map<std::string, vk::Device> const*, PhysicalDevice const*, vk::SurfaceKHR> getDeviceReferences();

		Instance() = default;
		~Instance();
		Instance(Instance&) = delete;
		Instance& operator=(Instance&) = delete;

	private:
		bool isSupported(std::vector<const char*> RequiredExtensions = {}, std::vector<const char*> RequiredLayers = {}) const;

		vk::Instance m_VulkanInstance;
		vk::SurfaceKHR m_Surface;

		PhysicalDevice m_PhysicalDevice{};
		std::unordered_map<std::string, vk::Device> m_LogicalDevices;

#ifndef NDEBUG
		vk::DispatchLoaderDynamic m_DLD_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}