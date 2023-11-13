#pragma once

#ifndef NDEBUG
#include "DebugMessenger.h"
#endif

#include "PhysicalDevice.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VT
{
	class Instance
	{
	public:
		void initInstance(const vk::ApplicationInfo& ApplicationInfo);

		void initDevice(GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues);

		vk::Device& getLogicalDevice();

		void destroy();

		Instance() {};
		Instance(Instance&) = delete;
		Instance& operator=(Instance&) = delete;

	private:
		bool isSupported(std::vector<const char*> RequiredExtensions = {}, std::vector<const char*> RequiredLayers = {}) const;

		vk::Instance m_VulkanInstance;
		vk::SurfaceKHR m_Surface;

		PhysicalDevice m_PhysicalDevice{};
		vk::Device m_LogicalDevice;

	#ifndef NDEBUG
		vk::DispatchLoaderDynamic m_DLD_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
	#endif

		friend class Renderer;
	};
}