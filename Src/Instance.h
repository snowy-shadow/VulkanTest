#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef NDEBUG
#include "DebugMessenger.h"
#endif

#include "PhysicalDevice.h"

namespace VT
{
	class Instance
	{
	public:
		void initInstance(const vk::ApplicationInfo& ApplicationInfo);

		void initDevice(GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues);

		vk::Device& getLogicalDevice();

		~Instance();

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