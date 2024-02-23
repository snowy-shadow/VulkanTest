#pragma once

#include "PhysicalDevice.h"

// must be included before all Windows API stuff
#include <vulkan/vulkan_structs.hpp>

namespace VT
{
	class Swapchain
	{
	public:
		struct Capabilities
		{
			uint32_t minImageCount;
			vk::Extent2D imageExtent;
			uint32_t arrayLayers;
			std::vector<vk::SurfaceFormatKHR> surfaceFormat;
			std::vector<vk::PresentModeKHR> presentMode;
			std::vector<vk::SurfaceTransformFlagBitsKHR> surfaceTransform;
			std::vector<vk::CompositeAlphaFlagBitsKHR> compositeAlpha;
			std::vector<vk::ImageUsageFlagBits> imageUsage;
		} m_SwapchainRequests;
	
		// fill this out
		vk::SwapchainCreateInfoKHR m_SwapchainInfo{};
		/*
		 * Finds and returns the first supported format for each. All vectors will have exactly 1 element when returned
		 * throws runtime error if none found
		*/
		VT::Swapchain::Capabilities queryCapabilities(PhysicalDevice const* PD, vk::SurfaceKHR Surface, Capabilities Capabilities) const;

		vk::SwapchainKHR getSwapchain() noexcept;
	
		void createSwapchain(vk::Device LogicalDevice);
		void destroySwapchain(vk::Device LogicalDevice);

	private:
		vk::SurfaceFormatKHR findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const;
		vk::PresentModeKHR findPresentMode(const std::vector<vk::PresentModeKHR>& SupportedPresentMode, const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const;

		// Obj
		vk::SwapchainKHR m_Swapchain;
	};
}
