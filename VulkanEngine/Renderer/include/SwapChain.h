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
		};

		/*
		 * Finds and replaces the first supported format for each in vk::SwapchainCreateInfoKHR.
		 * throws runtime error if none found
		*/
		void queryCapabilities(vk::SwapchainCreateInfoKHR&, Capabilities preferredCapabilities, vk::PhysicalDevice PD, vk::SurfaceKHR Surface) const;

		vk::SwapchainKHR getSwapchain() const noexcept;

		vk::SwapchainCreateInfoKHR getSwapchainCreateInfo() const noexcept;
	
		void createSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice);

		/**
		 * replace current swapchain with new one. sets SwapchainCreateInfo.oldSwapchin = current swapchain
		 * @param SwapchainCreateInfo : create info
		 * @param LogicalDevice : Logical device used to create swapchain
		 */
		void recreateSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice);

		vk::SurfaceFormatKHR findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const;
		vk::PresentModeKHR findPresentMode(const std::vector<vk::PresentModeKHR>& SupportedPresentMode, const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const;

		Swapchain(Swapchain&& Other) noexcept;
		Swapchain& operator = (Swapchain&& Other) noexcept;
		Swapchain operator = (Swapchain&) = delete;
		Swapchain(const Swapchain&) = delete;
		~Swapchain();

	private:
		
		vk::SwapchainCreateInfoKHR m_SwapchinCreateInfo;
		// Obj
		vk::SwapchainKHR m_Swapchain;
		vk::Device m_Device;
		bool Created{ false };
	};
}
