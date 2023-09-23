#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <unordered_set>
#include <string>
#include <array>
#include <tuple>
#include <optional>

namespace VT
{
	class PhysicalDevice
	{
	public:

		vk::Device createLogicalDevice(const std::vector<const char*>& DeviceExtensionName = {});

		void addQueue(const vk::QueueFlagBits& RequiredQueue = {}, const float& QueuePriority = 0.f, const uint32_t& QueueCount = 1);

		bool findPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList, const std::vector<const char*>& DeviceRequiredExtensions = {});

		bool findPresentQueue();

		void bindSurface(const vk::SurfaceKHR& Surface);

	private:
		bool extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions) const;

		std::array<uint32_t, 2> findQueue(const vk::QueueFlagBits& RequiredQueue, const uint32_t& QueueCount) const;

		std::vector<vk::DeviceQueueCreateInfo> m_DeviceQueues;

		vk::PhysicalDevice m_PhysicalDevice;
		vk::SurfaceKHR m_Surface;

		// Index, if found
		using Index = uint32_t;
		using IsGraphicsQueueIndex = bool;

		std::optional<Index> m_GraphicsQueue;
		std::tuple<Index, IsGraphicsQueueIndex> m_PresentQueue;



		// swapchain needs m_PhysicalDevice for query
		friend class SwapChain;
	};
}