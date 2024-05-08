#pragma once

#include "PhysicalDevice.h"

// must be included before all Windows API stuff
#include <vulkan/vulkan_structs.hpp>

namespace VT
{
	class Swapchain
	{
	public:
		Swapchain() = default;
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
		};

		/*
		 * Finds and returns the first supported format for each. All vectors will have exactly 1 element when returned
		 * throws runtime error if none found
		*/
		Capabilities queryCapabilities(Capabilities preferredCapabilities, PhysicalDevice const* PD, vk::SurfaceKHR Surface) const;

		vk::SwapchainKHR getSwapchain() const noexcept;

		vk::SwapchainCreateInfoKHR getSwapchainCreateInfo() const noexcept;
	
		void createSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice);
		void destroySwapchain(vk::Device LogicalDevice);

	private:
		vk::SurfaceFormatKHR findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const;
		vk::PresentModeKHR findPresentMode(const std::vector<vk::PresentModeKHR>& SupportedPresentMode, const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const;

		// Obj
		vk::SwapchainKHR m_Swapchain;

		vk::SwapchainCreateInfoKHR m_SwapchinCreateInfo;
	};
}
