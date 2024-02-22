#include "SwapChain.h"

#include <unordered_set>
#include <algorithm>

namespace VT
{
	void Swapchain::bindDevices(PhysicalDevice const* PD, vk::Device LD, vk::SurfaceKHR Surface)
	{
		m_Surface = Surface;
		m_LogicalDevice = LD;
		m_PhysicalDevice = PD;
	}

	bool Swapchain::deviceGraphicsQueueCanPresent() const noexcept { return m_PhysicalDevice->graphicsQueueCanPresent(); }
	
	vk::SwapchainKHR Swapchain::getSwapchain() noexcept { return m_Swapchain; }

	vk::SurfaceFormatKHR Swapchain::findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const
	{
		const auto SupportedSurfaceFormats = m_PhysicalDevice->getPhysicalDevice().getSurfaceFormatsKHR(m_Surface);
	
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

	vk::PresentModeKHR Swapchain::findPresentMode(const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const
	{
		const auto SupportedPresentation = m_PhysicalDevice->getPhysicalDevice().getSurfacePresentModesKHR(m_Surface);
		std::unordered_set<vk::PresentModeKHR> SPM{ SupportedPresentation.cbegin(), SupportedPresentation.cend() };

		for (const auto& PM : PreferredPresentModes)
		{
			if (SPM.contains(PM))
			{
				return PM;
			}
		}

		throw std::runtime_error("Unable to find Present mode");
	}

	VT::Swapchain::SurfaceCapabilities Swapchain::findSurfaceCapabilities(SurfaceCapabilities SC) const
	{
		const auto DeviceSurfaceCapabilities = m_PhysicalDevice->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface);

		SC.arrayLayers = std::clamp(SC.arrayLayers, SC.arrayLayers, DeviceSurfaceCapabilities.maxImageArrayLayers);
		
		// max Image count = 0 means no upper bound
		if (DeviceSurfaceCapabilities.maxImageCount > 0) { SC.minImageCount = std::clamp(SC.minImageCount, DeviceSurfaceCapabilities.minImageCount, DeviceSurfaceCapabilities.maxImageCount); }
		else { SC.minImageCount = SC.minImageCount < DeviceSurfaceCapabilities.minImageCount ? DeviceSurfaceCapabilities.minImageCount : SC.minImageCount; }

		// undefined surface size, set it to image size requested
		if (DeviceSurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
		{
			SC.imageExtent = vk::Extent2D
			{
				std::clamp(m_SwapchainInfo.imageExtent.width, DeviceSurfaceCapabilities.minImageExtent.width, DeviceSurfaceCapabilities.maxImageExtent.width),
				std::clamp(m_SwapchainInfo.imageExtent.height, DeviceSurfaceCapabilities.minImageExtent.height, DeviceSurfaceCapabilities.maxImageExtent.height)
			};
		}
		else { SC.imageExtent = DeviceSurfaceCapabilities.currentExtent; }

		// Composite alpha
		{
			auto Iterator
			{
				std::find_if(SC.compositeAlpha.cbegin(), SC.compositeAlpha.cend(),
				[&DeviceSurfaceCapabilities](const vk::CompositeAlphaFlagBitsKHR& PCA)
				{
					return DeviceSurfaceCapabilities.supportedCompositeAlpha & PCA;
				})
			};

			SC.compositeAlpha =
			{
				SC.compositeAlpha
				[
					Iterator != SC.compositeAlpha.cend() ?
					static_cast<uint32_t>(std::distance(SC.compositeAlpha.cbegin(), Iterator)) : throw std::runtime_error("Did not find required composite alpha")
				]
			};

		}

		// Surface transform
		{
			auto Iterator
			{
				std::find_if(SC.surfaceTransform.cbegin(), SC.surfaceTransform.cend(),
				[&DeviceSurfaceCapabilities](const vk::SurfaceTransformFlagBitsKHR& ST)
				{
					return DeviceSurfaceCapabilities.supportedTransforms & ST;
				})
			};


			SC.surfaceTransform = 
			{
				SC.surfaceTransform
				[
					Iterator != SC.surfaceTransform.cend() ?
					static_cast<uint32_t>(std::distance(SC.surfaceTransform.cbegin(), Iterator)) : throw std::runtime_error("Did not find required Surface Transform")
				]
			};
			
		}

		return SC;

	}

	void Swapchain::createSwapChain()
	{
		assert(m_Surface && m_PhysicalDevice && m_LogicalDevice);

		m_Swapchain = m_LogicalDevice.createSwapchainKHR(m_SwapchainInfo);
	}


	void Swapchain::destroySwapChain()
	{
		assert(m_Swapchain);
		m_LogicalDevice.destroySwapchainKHR(m_Swapchain);
	}

	/* ====================================================================
	*							Private
	* ====================================================================
	*/
}
