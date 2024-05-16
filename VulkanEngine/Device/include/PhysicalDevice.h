#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <optional>

namespace VT
{
	class PhysicalDevice
	{
	public:

		vk::Device createLogicalDevice(const std::vector<const char*>& DeviceExtensionName = {});

		vk::PhysicalDevice getPhysicalDevice() const;

		bool addQueue(const vk::QueueFlagBits& RequiredQueue, const float& QueuePriority = 1.f, const uint32_t& QueueCount = 1);

		bool findPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList, const std::vector<const char*>& DeviceRequiredExtensions);

        /*
          * Determines if extra device extensions will be required
          * DeviceRequiredExtensions will be modified if VK_KHR_portability_subset is supported
          *   The Vulkan spec states: If the VK_KHR_portability_subset extension is included in pProperties of vkEnumerateDeviceExtensionProperties,
          *   ppEnabledExtensionNames must include "VK_KHR_portability_subset"
          *   (https://vulkan.lunarg.com/doc/view/1.3.275.0/mac/1.3-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451)
        */
        bool supportsPortabilitySubset() const;
		bool findPresentQueue(const vk::SurfaceKHR& Surface, const float& PresentQPriority, const uint32_t& MinPresentQCount);

		bool findGraphicsQueueWithPresent(
			const vk::SurfaceKHR& Surface, 
			const float& GraphicsQPriority, const uint32_t& MinGraphicsQCount, 
			const float& PresentQPriority, const uint32_t& MinPresentQCount);

		/**
		 * 
		 * @return { queueFamilyIndex, PresentQueue index }
		 */
		std::pair<uint32_t, uint32_t> getGraphicsPresentQueueIndices() const;

		bool graphicsQueueCanPresent() const;

		/**
		 * get Device queues
		 */
		std::vector<vk::DeviceQueueCreateInfo> getDeviceQueues() const;

		PhysicalDevice() = default;
		PhysicalDevice(PhysicalDevice&) = delete;
		PhysicalDevice& operator = (PhysicalDevice) = delete;

	private:
		static bool extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions);

		uint32_t m_PresentQueue{};
		bool m_GraphicsCanPresent = false;

		vk::PhysicalDevice m_PhysicalDevice;
		std::vector<vk::DeviceQueueCreateInfo> m_DeviceQueues;
		std::optional<vk::DeviceQueueCreateInfo> m_GraphicsQueue;
	};
}
