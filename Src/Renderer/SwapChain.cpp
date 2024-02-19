#include "SwapChain.h"

#include <unordered_set>
#include <algorithm>

namespace VT
{
	SwapChain::SwapChain(const std::array<uint32_t, 2>& WidthHeight, const uint32_t& ImageAmount)
	{
		m_WidthHeight = WidthHeight;
		m_ImageCount = ImageAmount;
	}

	void SwapChain::createSwapChain(const vk::SwapchainKHR& Old)
	{
		assert(m_Surface && m_PhysicalDevice && m_LogicalDevice);

		vk::SwapchainCreateInfoKHR SC_Info
		{
			.surface = m_Surface,
			.minImageCount = m_ImageCount,
			.imageFormat = m_SurfaceFormat.format,
			.imageColorSpace = m_SurfaceFormat.colorSpace,
			.imageExtent = m_SurfaceCapabilities.currentExtent,
			.imageArrayLayers = m_ArrayLayers,
            // TODO put all settings in a struct, expose to user, no need for this class probably
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = m_SharingMode,
			.presentMode = m_PresentMode,
			.oldSwapchain = Old
		};

		if(!m_PhysicalDevice->GraphicsCanPresent())
		{
			SC_Info.imageSharingMode = vk::SharingMode::eConcurrent;
			SC_Info.queueFamilyIndexCount = static_cast<uint32_t>(m_PhysicalDevice->getGraphicsPresentQueueIndices().size());
			SC_Info.pQueueFamilyIndices = m_PhysicalDevice->getGraphicsPresentQueueIndices().data();
		}

		m_SwapChain = m_LogicalDevice.createSwapchainKHR(SC_Info);
	}

	void SwapChain::setProperties(
		const std::vector<vk::SurfaceFormatKHR>& PreferredFormats, 
		const std::vector<vk::PresentModeKHR>& PreferredPresentations, 
		const std::vector<vk::CompositeAlphaFlagBitsKHR>& PreferredCompositeAlpha, 
		const std::vector<vk::SurfaceTransformFlagBitsKHR>& PreferredSurfaceTransform)
	{
		vk::PhysicalDevice PD = m_PhysicalDevice->getPhysicalDevice();

		m_SurfaceFormat = findSurfaceFormat(PD.getSurfaceFormatsKHR(m_Surface), PreferredFormats);
		m_PresentMode = findPresentMode(PD.getSurfacePresentModesKHR(m_Surface), PreferredPresentations);

		m_SurfaceCapabilities = findSurfaceCapabilities(PD.getSurfaceCapabilitiesKHR(m_Surface), PreferredCompositeAlpha, PreferredSurfaceTransform);

		// defined minImageCount == maxImageCount in m_SurfaceCapabilities
		m_ImageCount = m_SurfaceCapabilities.minImageCount;
		m_ArrayLayers = m_SurfaceCapabilities.maxImageArrayLayers;
	}

	void SwapChain::setImageCount(const uint32_t& Amount) { m_ImageCount = Amount; }

	void SwapChain::bindDevice(PhysicalDevice const* PD, vk::Device LD, vk::SurfaceKHR Surface)
	{
		m_Surface = Surface;
		m_LogicalDevice = LD;
		m_PhysicalDevice = PD;
	}

	std::vector<vk::Image> SwapChain::getSwapChainImages()
	{
		return m_LogicalDevice.getSwapchainImagesKHR(m_SwapChain);
	}

	void SwapChain::destroySwapChain()
	{
		m_LogicalDevice.destroySwapchainKHR(m_SwapChain);
	}

	/* ====================================================================
	*							Private
	* ====================================================================
	*/

	vk::SurfaceFormatKHR SwapChain::findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const
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

	vk::PresentModeKHR SwapChain::findPresentMode(const std::vector<vk::PresentModeKHR>& SupportedPresentation, const std::vector<vk::PresentModeKHR>& PreferredPresentModes) const
	{
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

	vk::SurfaceCapabilitiesKHR SwapChain::findSurfaceCapabilities(
		const vk::SurfaceCapabilitiesKHR& SurfaceCapabilities,
		const std::vector<vk::CompositeAlphaFlagBitsKHR>& PreferredCompositeAlpha,
		const std::vector<vk::SurfaceTransformFlagBitsKHR>& PreferredSurfaceTransform) const
	{
		auto SC = SurfaceCapabilities;
		
		// max Image count = 0 means no upper bound
		if (SurfaceCapabilities.maxImageCount > 0) { SC.minImageCount = std::clamp(m_ImageCount, SurfaceCapabilities.minImageCount, SurfaceCapabilities.maxImageCount); }
		else { SC.minImageCount = m_ImageCount < SurfaceCapabilities.minImageCount ? SurfaceCapabilities.minImageCount : m_ImageCount; }

		SC.maxImageCount = SC.minImageCount;

		// undefined surface size, set it to image size requested
		if (SurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
		{
			SC.currentExtent = vk::Extent2D
			{
				std::clamp(m_WidthHeight.at(0), SurfaceCapabilities.minImageExtent.width, SurfaceCapabilities.maxImageExtent.width),
				std::clamp(m_WidthHeight.at(1), SurfaceCapabilities.minImageExtent.height, SurfaceCapabilities.maxImageExtent.height)
			};
		}

		// Composite alpha
		{
			auto Iterator
			{
				std::find_if(PreferredCompositeAlpha.cbegin(), PreferredCompositeAlpha.cend(),
				[&SurfaceCapabilities](const vk::CompositeAlphaFlagBitsKHR& PCA)
				{
					return SurfaceCapabilities.supportedCompositeAlpha & PCA;
				})
			};

			SC.supportedCompositeAlpha = PreferredCompositeAlpha.at
			(
				Iterator != PreferredCompositeAlpha.cend() ?
				static_cast<uint32_t>(std::distance(PreferredCompositeAlpha.cbegin(), Iterator)) : throw std::runtime_error("Did not find required composite alpha")
			);

		}

		// Surface transform
		{
			auto Iterator
			{
				std::find_if(PreferredSurfaceTransform.cbegin(), PreferredSurfaceTransform.cend(),
				[&SurfaceCapabilities](const vk::SurfaceTransformFlagBitsKHR& ST)
				{
					return SurfaceCapabilities.supportedTransforms & ST;
				})
			};


			SC.supportedTransforms = PreferredSurfaceTransform.at
			(
				Iterator != PreferredSurfaceTransform.cend() ?
				static_cast<uint32_t>(std::distance(PreferredSurfaceTransform.cbegin(), Iterator)) : throw std::runtime_error("Did not find required Surface Transform")
			);
			
		}

		return SC;

	}
}
