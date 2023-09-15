#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <unordered_set>

#ifndef NDEBUG
#include "DebugMessenger.h"
#endif

#include "PhysicalDevice.h"

namespace VT
{
	class Instance
	{
	public:
		void initInstance(const vk::ApplicationInfo ApplicationInfo);
		vk::Instance& getInstance();

		vk::Result CreateWindowSurface(GLFWwindow* Window);

		~Instance();
	private:

		bool isSupported(std::vector<const char*> RequiredExtensions = {}, std::vector<const char*> RequiredLayers = {}) const;

		vk::Instance m_VulkanInstance;
		vk::SurfaceKHR m_Surface{};

		PhysicalDevice m_PhysicalDevice;
		vk::Device m_LogicalDevice;

	#ifndef NDEBUG
		vk::DispatchLoaderDynamic m_DLD_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
	#endif

	};
}