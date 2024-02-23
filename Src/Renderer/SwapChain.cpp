#include "Swapchain.h"

#include <unordered_set>
#include <algorithm>

#include <vulkan/vulkan_hash.hpp>

namespace VT
{
	vk::SwapchainKHR Swapchain::getSwapchain() noexcept { return m_Swapchain; }


	Swapchain::Capabilities Swapchain::queryCapabilities(PhysicalDevice const* PD, vk::SurfaceKHR Surface, Capabilities Capabilities) const
	{
		Capabilities.surfaceFormat = {findSurfaceFormat(PD->getPhysicalDevice().getSurfaceFormatsKHR(Surface), Capabilities.surfaceFormat)};
		Capabilities.presentMode = {findPresentMode(PD->getPhysicalDevice().getSurfacePresentModesKHR(Surface), Capabilities.presentMode)};

		const auto DeviceSurfaceCapabilities = PD->getPhysicalDevice().getSurfaceCapabilitiesKHR(Surface);

		Capabilities.arrayLayers = std::clamp(Capabilities.arrayLayers, Capabilities.arrayLayers, DeviceSurfaceCapabilities.maxImageArrayLayers);
		
		// max Image count = 0 means no upper bound
		if (DeviceSurfaceCapabilities.maxImageCount > 0) { Capabilities.minImageCount = std::clamp(Capabilities.minImageCount, DeviceSurfaceCapabilities.minImageCount, DeviceSurfaceCapabilities.maxImageCount); }
		else { Capabilities.minImageCount = Capabilities.minImageCount < DeviceSurfaceCapabilities.minImageCount ? DeviceSurfaceCapabilities.minImageCount : Capabilities.minImageCount; }

		// undefined surface size, set it to image size requested
		if (DeviceSurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
		{
			Capabilities.imageExtent = vk::Extent2D
			{
				std::clamp(Capabilities.imageExtent.width, DeviceSurfaceCapabilities.minImageExtent.width, DeviceSurfaceCapabilities.maxImageExtent.width),
				std::clamp(Capabilities.imageExtent.height, DeviceSurfaceCapabilities.minImageExtent.height, DeviceSurfaceCapabilities.maxImageExtent.height)
			};
		}
		else { Capabilities.imageExtent = DeviceSurfaceCapabilities.currentExtent; }

		// Composite alpha
		{
			auto Iterator
			{
				std::find_if(Capabilities.compositeAlpha.cbegin(), Capabilities.compositeAlpha.cend(),
				[&DeviceSurfaceCapabilities](const vk::CompositeAlphaFlagBitsKHR& PCA)
				{
					return DeviceSurfaceCapabilities.supportedCompositeAlpha & PCA;
				})
			};

			Capabilities.compositeAlpha =
			{
				Capabilities.compositeAlpha
				[
					Iterator != Capabilities.compositeAlpha.cend() ?
					static_cast<uint32_t>(std::distance(Capabilities.compositeAlpha.cbegin(), Iterator)) : throw std::runtime_error("Did not find required composite alpha")
				]
			};

		}

		// Surface transform
		{
			auto Iterator
			{
				std::find_if(Capabilities.surfaceTransform.cbegin(), Capabilities.surfaceTransform.cend(),
				[&DeviceSurfaceCapabilities](const vk::SurfaceTransformFlagBitsKHR& ST)
				{
					return DeviceSurfaceCapabilities.supportedTransforms & ST;
				})
			};


			Capabilities.surfaceTransform = 
			{
				Capabilities.surfaceTransform
				[
					Iterator != Capabilities.surfaceTransform.cend() ?
					static_cast<uint32_t>(std::distance(Capabilities.surfaceTransform.cbegin(), Iterator)) : throw std::runtime_error("Did not find required Surface Transform")
				]
			};
			
		}

		return Capabilities;

	}

	void Swapchain::createSwapchain(vk::Device LogicalDevice)
	{
		m_Swapchain = LogicalDevice.createSwapchainKHR(m_SwapchainInfo);
	}


	void Swapchain::destroySwapchain(vk::Device LogicalDevice)
	{
		assert(m_Swapchain);
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
