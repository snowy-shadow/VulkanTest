#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <unordered_set>
#include <string>
#include <array>
#include <tuple>

namespace VT
{
	class PhysicalDevice
	{
	public:

		PhysicalDevice(const vk::SurfaceKHR& Surface) : m_Surface(Surface) {};

		vk::Device createLogicalDevice(
			const std::vector<vk::PhysicalDevice>& DeviceList,
			const std::vector<const char*>& RequiredExtensions = {},
			const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues = {},
			const vk::SurfaceKHR& Surface);

		void addQueue(const vk::QueueFlagBits& RequiredQueue = {}, const float& QueuePriority = 0.f, const uint32_t& QueueCount = 1);


	private:
		bool findPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList,
			const std::vector<const char*>& RequiredExtensions);

		bool extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions) const;

		uint32_t findPresentQueueIndex(const vk::SurfaceKHR& Surface) const;

		std::array<uint32_t, 2> findQueue(const vk::QueueFlagBits& RequiredQueue, const uint32_t& QueueCount);

		std::vector<vk::DeviceQueueCreateInfo> m_DeviceQueues;

		vk::PhysicalDevice m_PhysicalDevice;

		vk::SurfaceKHR m_Surface;

		std::tuple<uint32_t, bool> m_PresentQueueInfo;
	};
}