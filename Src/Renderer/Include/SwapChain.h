#pragma once

#include "PhysicalDevice.h"

// must be included before all Windows API stuff
#include <vulkan/vulkan_hash.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace VT
{
	class Swapchain
	{
	public:
		struct SurfaceCapabilities
		{
			uint32_t minImageCount;
			vk::Extent2D imageExtent;
			uint32_t arrayLayers;
			std::vector<vk::SurfaceTransformFlagBitsKHR> surfaceTransform;
			std::vector<vk::CompositeAlphaFlagBitsKHR> compositeAlpha;
			std::vector<vk::ImageUsageFlagBits> imageUsage;
		};
	
		// bind devices before creation
		void bindDevices(PhysicalDevice const* PD, vk::Device LD, vk::SurfaceKHR Surface);
	
		// fill this out
		vk::SwapchainCreateInfoKHR m_SwapchainInfo{};

		void createSwapChain();
		void destroySwapChain();

		bool deviceGraphicsQueueCanPresent() const noexcept;
		vk::SwapchainKHR getSwapchain() noexcept;
		/*
		 * Finds and returns the first supported format
		 * throws runtime error if none found
		*/
		vk::SurfaceFormatKHR findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const;
		vk::PresentModeKHR findPresentMode(const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const;
	
		VT::Swapchain::SurfaceCapabilities findSurfaceCapabilities(SurfaceCapabilities SC) const;

	private:
		// Obj
		vk::SwapchainKHR m_Swapchain;

		// Handle to base, keep track of where swapchain is created
		vk::Device m_LogicalDevice{};
		PhysicalDevice const* m_PhysicalDevice{nullptr};
		vk::SurfaceKHR m_Surface{};
	};
}
