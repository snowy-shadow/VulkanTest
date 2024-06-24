#pragma once


#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// enable aggregate initialization
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	class Instance
	{
	public:
		/*
		 * Create vulkan instance
		 */
		void initInstance(const vk::ApplicationInfo& ApplicationInfo, std::vector<const char*> RequiredExtensions, std::vector<const char*> RequiredLayers);

		vk::Instance getInstance() const noexcept;

		Instance() = default;
		~Instance();
		Instance(Instance&) = delete;
		Instance& operator=(Instance&) = delete;

	private:
		bool static isSupported(std::span<const char*> RequiredExtensions = {}, std::span<const char*> RequiredLayers = {});

		vk::Instance m_VulkanInstance;

#ifndef NDEBUG
		vk::DispatchLoaderDynamic m_DLD_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}