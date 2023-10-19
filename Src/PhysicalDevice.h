#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <unordered_set>
#include <array>
#include <optional>

namespace VT
{
	class PhysicalDevice
	{
	public:

		vk::Device createLogicalDevice(const std::vector<const char*>& DeviceExtensionName = {});

		vk::PhysicalDevice& getPhysicalDevice();

		bool addQueue(const vk::QueueFlagBits& RequiredQueue, const float& QueuePriority = 1.f, const uint32_t& QueueCount = 1);

		bool findPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList, const std::vector<const char*>& DeviceRequiredExtensions = {});

		bool findPresentQueue(const vk::SurfaceKHR& Surface, const float& PresentQPriority, const uint32_t& MinPresentQCount);

		bool findGraphicsQueueWithPresent(
			const vk::SurfaceKHR& Surface, 
			const float& GraphicsQPriority, const uint32_t& MinGraphicsQCount, 
			const float& PresentQPriority, const uint32_t& MinPresentQCount);

		std::array<uint32_t, 2> getGraphicsPresentQueueIndices();

		bool m_GraphicsCanPresent = false;

		PhysicalDevice() = default;
		PhysicalDevice(PhysicalDevice&) = delete;
		PhysicalDevice& operator = (PhysicalDevice) = delete;

	private:
		static bool extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions);

		vk::PhysicalDevice m_PhysicalDevice;
		std::vector<vk::DeviceQueueCreateInfo> m_DeviceQueues;
		std::optional<vk::DeviceQueueCreateInfo> m_GraphicsQueue;
		uint32_t m_PresentQueue;
	};
}