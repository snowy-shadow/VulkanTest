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

		bool findPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList, const std::vector<const char*>& DeviceRequiredExtensions = {});

		bool findPresentQueue(const vk::SurfaceKHR& Surface, const float& PresentQPriority, const uint32_t& MinPresentQCount);

		bool findGraphicsQueueWithPresent(
			const vk::SurfaceKHR& Surface, 
			const float& GraphicsQPriority, const uint32_t& MinGraphicsQCount, 
			const float& PresentQPriority, const uint32_t& MinPresentQCount);

		std::array<uint32_t, 2> getGraphicsPresentQueueIndices() const;

		bool GraphicsCanPresent() const;

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