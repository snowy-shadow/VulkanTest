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

		vk::Device createPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList,
			const std::vector<const char*>& RequiredExtensions = {},
			const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues = {});

		void addQueue(const vk::QueueFlagBits& RequiredQueue = {}, const float& QueuePriority = 0.f, const uint32_t& QueueCount = 1);


	private:
		bool findDevice(const std::vector<vk::PhysicalDevice>& DeviceList,
			const std::vector<const char*>& RequiredExtensions);

		bool extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions) const;

		std::array<uint32_t, 2> findQueue(const vk::QueueFlagBits& RequiredQueue, const uint32_t& QueueCount) const;

		std::vector<vk::DeviceQueueCreateInfo> m_DeviceQueues;

		vk::PhysicalDevice m_PhysicalDevice;
	};
}