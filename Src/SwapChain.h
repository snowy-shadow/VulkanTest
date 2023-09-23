#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "PhysicalDevice.h"

namespace VT
{
	class SwapChain
	{
	public:
		void createSwapChain(const PhysicalDevice& PD);

		void setImageFormat(const vk::Format& Format = vk::Format::eB8G8R8A8Unorm);
		void setImageCount(const uint32_t& Amount = 1);
		void setMaxWidthHeight(const uint32_t& Width = 1920, const uint32_t& Height = 1080);


	private:
		uint32_t m_ImageCount;
		uint32_t m_Width, m_Height;

		vk::SurfaceCapabilitiesKHR m_SurfaceCapabilities;
		vk::Format m_SurfaceFormat;
		vk::PresentModeKHR m_PresentMode;

		vk::SwapchainKHR m_SwapChain;
	};
}
