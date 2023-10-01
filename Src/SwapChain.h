#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.h"

namespace VT
{
	class SwapChain
	{
	public:
		void createSwapChain(const PhysicalDevice& PD,
			const std::vector<vk::SurfaceFormatKHR>& PreferedFormats,
			const std::vector<vk::PresentModeKHR>& PreferedPresentations);

		void setImageCount(const uint32_t& Amount);
		void setWidthHeight(const std::array<uint32_t, 2> XY);


	private:
		bool FormatSupported(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& SurfaceFormat);
		bool PresentModeSupported(const std::vector<vk::PresentModeKHR>& SupportedPresentation, const std::vector<vk::PresentModeKHR>& PresentModes);


		uint32_t m_ImageCount = 2;
		std::array<uint32_t, 2> m_WidthHeight{ 1920, 1080 };

		vk::SurfaceCapabilitiesKHR m_SurfaceCapabilities;
		vk::SurfaceFormatKHR m_SurfaceFormat;
		vk::PresentModeKHR m_PresentMode;

		vk::SwapchainKHR m_SwapChain;
	};
}
