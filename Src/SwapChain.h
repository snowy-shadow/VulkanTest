#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_hash.hpp>

#include "Instance.h"

namespace VT
{
	class SwapChain
	{
	public:
		SwapChain(const std::array<uint32_t, 2>& WidthHeight, const uint32_t& ImageAmount);

		void setImageCount(const uint32_t& Amount);

		void bindDevice(const PhysicalDevice&, const vk::Device&, const vk::SurfaceKHR&);

		void createSwapChain(const vk::SwapchainKHR& Old = nullptr);

		void setProperties
		(
			const std::vector<vk::SurfaceFormatKHR>& PreferredFormats,
			const std::vector<vk::PresentModeKHR>& PreferredPresentations,
			const std::vector<vk::CompositeAlphaFlagBitsKHR>& PreferredCompositeAlpha,
			const std::vector<vk::SurfaceTransformFlagBitsKHR>& PreferredSurfaceTransform
		);

		std::vector<vk::Image> getSwapChainImages();
		void destroySwapChain();

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator = (const SwapChain&) = delete;

	private:
		vk::SurfaceFormatKHR findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& SurfaceFormat) const;
		vk::PresentModeKHR findPresentMode(const std::vector<vk::PresentModeKHR>& SupportedPresentation, const std::vector<vk::PresentModeKHR>& PresentModes) const;

		vk::SurfaceCapabilitiesKHR findSurfaceCapabilities(const vk::SurfaceCapabilitiesKHR& SurfaceCapabilities,
			const std::vector<vk::CompositeAlphaFlagBitsKHR>& PreferredCompositeAlpha,
			const std::vector<vk::SurfaceTransformFlagBitsKHR>& PreferredSurfaceTransform) const;

		// Configs
		uint32_t m_ImageCount;
		uint32_t m_ArrayLayers;
		std::array<uint32_t, 2> m_WidthHeight{ 1920, 1080 };
		vk::SharingMode m_SharingMode = vk::SharingMode::eExclusive;

		vk::SurfaceCapabilitiesKHR m_SurfaceCapabilities;
		vk::SurfaceFormatKHR m_SurfaceFormat;
		vk::PresentModeKHR m_PresentMode;

		// Obj
		vk::SwapchainKHR m_SwapChain;

		// Instance
		const vk::Device* m_LogicalDevice{};
		const PhysicalDevice* m_PhysicalDevice{};
		const vk::SurfaceKHR* m_Surface{};
	};
}
