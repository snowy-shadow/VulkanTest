#include "SwapChain.h"
#include "Swapchain.h"

#include <unordered_set>
#include <algorithm>

#include <vulkan/vulkan_hash.hpp>

namespace VT
{
	Swapchain::Capabilities Swapchain::queryCapabilities(Capabilities preferredCapabilities, PhysicalDevice const* PD,
		vk::SurfaceKHR Surface) const
	{
		preferredCapabilities.surfaceFormat = { findSurfaceFormat(PD->getPhysicalDevice().getSurfaceFormatsKHR(Surface), preferredCapabilities.surfaceFormat) };
		preferredCapabilities.presentMode = { findPresentMode(PD->getPhysicalDevice().getSurfacePresentModesKHR(Surface), preferredCapabilities.presentMode) };

		const auto DeviceSurfaceCapabilities = PD->getPhysicalDevice().getSurfaceCapabilitiesKHR(Surface);

		preferredCapabilities.arrayLayers = std::clamp(preferredCapabilities.arrayLayers, preferredCapabilities.arrayLayers, DeviceSurfaceCapabilities.maxImageArrayLayers);

		// max Image count = 0 means no upper bound
		if (DeviceSurfaceCapabilities.maxImageCount > 0) 
		{ preferredCapabilities.minImageCount = std::clamp(preferredCapabilities.minImageCount, DeviceSurfaceCapabilities.minImageCount, DeviceSurfaceCapabilities.maxImageCount); }
		else { preferredCapabilities.minImageCount = preferredCapabilities.minImageCount < DeviceSurfaceCapabilities.minImageCount ? DeviceSurfaceCapabilities.minImageCount : preferredCapabilities.minImageCount; }

		// undefined surface size, set it to image size requested
		if (DeviceSurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
		{
			preferredCapabilities.imageExtent =
			{
				std::clamp(preferredCapabilities.imageExtent.width, DeviceSurfaceCapabilities.minImageExtent.width, DeviceSurfaceCapabilities.maxImageExtent.width),
				std::clamp(preferredCapabilities.imageExtent.height, DeviceSurfaceCapabilities.minImageExtent.height, DeviceSurfaceCapabilities.maxImageExtent.height)
			};
		}
		else { preferredCapabilities.imageExtent = DeviceSurfaceCapabilities.currentExtent; }

		// Composite alpha
		{
			const auto Iterator
			{
				std::find_if(preferredCapabilities.compositeAlpha.cbegin(), preferredCapabilities.compositeAlpha.cend(),
				[&DeviceSurfaceCapabilities](const vk::CompositeAlphaFlagBitsKHR& PCA)
				{
					return DeviceSurfaceCapabilities.supportedCompositeAlpha & PCA;
				})
			};

			preferredCapabilities.compositeAlpha =
			{
				preferredCapabilities.compositeAlpha
				[
					Iterator != preferredCapabilities.compositeAlpha.cend() ?
					static_cast<uint32_t>(std::distance(preferredCapabilities.compositeAlpha.cbegin(), Iterator)) : throw std::runtime_error("Did not find required composite alpha")
				]
			};

		}

		// Surface transform
		{
			const auto Iterator
			{
				std::find_if(preferredCapabilities.surfaceTransform.cbegin(), preferredCapabilities.surfaceTransform.cend(),
				[&DeviceSurfaceCapabilities](const vk::SurfaceTransformFlagBitsKHR& ST)
				{
					return DeviceSurfaceCapabilities.supportedTransforms & ST;
				})
			};


			preferredCapabilities.surfaceTransform =
			{
				preferredCapabilities.surfaceTransform
				[
					Iterator != preferredCapabilities.surfaceTransform.cend() ?
					static_cast<uint32_t>(std::distance(preferredCapabilities.surfaceTransform.cbegin(), Iterator)) : throw std::runtime_error("Did not find required Surface Transform")
				]
			};

		}

		return preferredCapabilities;
	}

	vk::SwapchainKHR Swapchain::getSwapchain() const noexcept { return m_Swapchain; }

	vk::SwapchainCreateInfoKHR Swapchain::getSwapchainCreateInfo() const noexcept { return m_SwapchinCreateInfo; }

	void Swapchain::createSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice)
	{
		m_Swapchain = LogicalDevice.createSwapchainKHR(SwapchainCreateInfo);
		m_SwapchinCreateInfo = std::move(SwapchainCreateInfo);
	}

	void Swapchain::destroySwapchain(vk::Device LogicalDevice)
	{
		LogicalDevice.destroySwapchainKHR(m_Swapchain);
	}

	/* ====================================================================
	*							Private
	* ====================================================================
	*/

	vk::SurfaceFormatKHR Swapchain::findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const
	{
		std::unordered_set<vk::SurfaceFormatKHR> SSF{ SupportedSurfaceFormats.cbegin(), SupportedSurfaceFormats.cend() };

		for (const auto& SF : PreferredSurfaceFormat)
		{
			if (SSF.contains(SF))
			{
				return SF;
			}
		}

		throw std::runtime_error("Unable to find SurfaceFormat");
	}

	vk::PresentModeKHR Swapchain::findPresentMode(const std::vector<vk::PresentModeKHR>& SupportedPresentMode, const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const
	{
		std::unordered_set<vk::PresentModeKHR> SPM{ SupportedPresentMode.cbegin(), SupportedPresentMode.cend() };

		for (const auto& PM : PreferredPresentModes)
		{
			if (SPM.contains(PM))
			{
				return PM;
			}
		}

		throw std::runtime_error("Unable to find Present mode");
	}
}
