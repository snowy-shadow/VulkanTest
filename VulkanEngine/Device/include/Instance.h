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
		bool isSupported(std::vector<const char*> RequiredExtensions = {}, std::vector<const char*> RequiredLayers = {}) const;

		enum ObjectConstructed : uint8_t
		{
			eInstance = 0b1,
#ifndef NDEBUG
			eDebugMessenger = 0b10,
#endif
		};
		uint8_t m_ObjectConstructedMask{0};
	
		vk::Instance m_VulkanInstance;

#ifndef NDEBUG
		vk::DispatchLoaderDynamic m_DLD_Instance;
		vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
	};
}
