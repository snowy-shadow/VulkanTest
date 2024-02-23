#include "Swapchain.h"

#include <unordered_set>
#include <algorithm>

#include <vulkan/vulkan_hash.hpp>

namespace VT
{
	vk::SwapchainKHR Swapchain::getSwapchain() noexcept { return m_Swapchain; }

	void Swapchain::queryCapabilities(PhysicalDevice const* PD, vk::SurfaceKHR Surface)
	{
		m_SwapchainRequest.surfaceFormat = {findSurfaceFormat(PD->getPhysicalDevice().getSurfaceFormatsKHR(Surface), m_SwapchainRequest.surfaceFormat)};
		m_SwapchainRequest.presentMode = {findPresentMode(PD->getPhysicalDevice().getSurfacePresentModesKHR(Surface), m_SwapchainRequest.presentMode)};

		const auto DeviceSurfaceCapabilities = PD->getPhysicalDevice().getSurfaceCapabilitiesKHR(Surface);

		m_SwapchainRequest.arrayLayers = std::clamp(m_SwapchainRequest.arrayLayers, m_SwapchainRequest.arrayLayers, DeviceSurfaceCapabilities.maxImageArrayLayers);
		
		// max Image count = 0 means no upper bound
		if (DeviceSurfaceCapabilities.maxImageCount > 0) { m_SwapchainRequest.minImageCount = std::clamp(m_SwapchainRequest.minImageCount, DeviceSurfaceCapabilities.minImageCount, DeviceSurfaceCapabilities.maxImageCount); }
		else { m_SwapchainRequest.minImageCount = m_SwapchainRequest.minImageCount < DeviceSurfaceCapabilities.minImageCount ? DeviceSurfaceCapabilities.minImageCount : m_SwapchainRequest.minImageCount; }

		// undefined surface size, set it to image size requested
		if (DeviceSurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
		{
			m_SwapchainRequest.imageExtent = vk::Extent2D
			{
				std::clamp(m_SwapchainRequest.imageExtent.width, DeviceSurfaceCapabilities.minImageExtent.width, DeviceSurfaceCapabilities.maxImageExtent.width),
				std::clamp(m_SwapchainRequest.imageExtent.height, DeviceSurfaceCapabilities.minImageExtent.height, DeviceSurfaceCapabilities.maxImageExtent.height)
			};
		}
		else { m_SwapchainRequest.imageExtent = DeviceSurfaceCapabilities.currentExtent; }

		// Composite alpha
		{
			auto Iterator
			{
				std::find_if(m_SwapchainRequest.compositeAlpha.cbegin(), m_SwapchainRequest.compositeAlpha.cend(),
				[&DeviceSurfaceCapabilities](const vk::CompositeAlphaFlagBitsKHR& PCA)
				{
					return DeviceSurfaceCapabilities.supportedCompositeAlpha & PCA;
				})
			};

			m_SwapchainRequest.compositeAlpha =
			{
				m_SwapchainRequest.compositeAlpha
				[
					Iterator != m_SwapchainRequest.compositeAlpha.cend() ?
					static_cast<uint32_t>(std::distance(m_SwapchainRequest.compositeAlpha.cbegin(), Iterator)) : throw std::runtime_error("Did not find required composite alpha")
				]
			};

		}

		// Surface transform
		{
			auto Iterator
			{
				std::find_if(m_SwapchainRequest.surfaceTransform.cbegin(), m_SwapchainRequest.surfaceTransform.cend(),
				[&DeviceSurfaceCapabilities](const vk::SurfaceTransformFlagBitsKHR& ST)
				{
					return DeviceSurfaceCapabilities.supportedTransforms & ST;
				})
			};


			m_SwapchainRequest.surfaceTransform = 
			{
				m_SwapchainRequest.surfaceTransform
				[
					Iterator != m_SwapchainRequest.surfaceTransform.cend() ?
					static_cast<uint32_t>(std::distance(m_SwapchainRequest.surfaceTransform.cbegin(), Iterator)) : throw std::runtime_error("Did not find required Surface Transform")
				]
			};
			
		}

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
