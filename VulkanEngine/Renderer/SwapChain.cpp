#include "Swapchain.h"

#include <unordered_set>
#include <algorithm>

#include <vulkan/vulkan_hash.hpp>

namespace VT
{
	void Swapchain::queryCapabilities(vk::SwapchainCreateInfoKHR& Info, Capabilities preferredCapabilities, vk::PhysicalDevice PD,
		vk::SurfaceKHR Surface) const
	{
		Info.surface = Surface;

		// Surface format
		const auto SurfaceFormat = findSurfaceFormat(PD.getSurfaceFormatsKHR(Surface), preferredCapabilities.surfaceFormat);
		Info.imageFormat = SurfaceFormat.format;
		Info.imageColorSpace = SurfaceFormat.colorSpace;

		// present mode
		Info.presentMode = findPresentMode(PD.getSurfacePresentModesKHR(Surface), preferredCapabilities.presentMode);

		// image count
		const auto& DeviceSurfaceCapabilities = PD.getSurfaceCapabilitiesKHR(Surface);
		// max Image count = 0 means no upper bound
		if (DeviceSurfaceCapabilities.maxImageCount > 0)
		{
			Info.minImageCount = std::clamp(preferredCapabilities.minImageCount, DeviceSurfaceCapabilities.minImageCount, DeviceSurfaceCapabilities.maxImageCount);
		}
		else if (preferredCapabilities.minImageCount < DeviceSurfaceCapabilities.minImageCount)
		{
			Info.minImageCount = DeviceSurfaceCapabilities.minImageCount;
		}

		// Image extent
		// undefined surface size, set it to image size requested
		if (DeviceSurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
		{
			Info.imageExtent =
			{
				std::clamp(preferredCapabilities.imageExtent.width, DeviceSurfaceCapabilities.minImageExtent.width, DeviceSurfaceCapabilities.maxImageExtent.width),
				std::clamp(preferredCapabilities.imageExtent.height, DeviceSurfaceCapabilities.minImageExtent.height, DeviceSurfaceCapabilities.maxImageExtent.height)
			};
		}
		else { Info.imageExtent = DeviceSurfaceCapabilities.currentExtent; }

		// Array layers
		Info.imageArrayLayers = std::clamp(preferredCapabilities.arrayLayers, preferredCapabilities.arrayLayers, DeviceSurfaceCapabilities.maxImageArrayLayers);

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

			Info.compositeAlpha =
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


			Info.preTransform =
			{
				preferredCapabilities.surfaceTransform
				[
					Iterator != preferredCapabilities.surfaceTransform.cend() ?
					static_cast<uint32_t>(std::distance(preferredCapabilities.surfaceTransform.cbegin(), Iterator)) : throw std::runtime_error("Did not find required Surface Transform")
				]
			};

		}
	}

	vk::SwapchainKHR Swapchain::getSwapchain() const noexcept { return m_Swapchain; }

	vk::SwapchainCreateInfoKHR Swapchain::getSwapchainCreateInfo() const noexcept { return m_SwapchinCreateInfo; }

	void Swapchain::createSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice)
	{
		SwapchainCreateInfo.oldSwapchain = m_Swapchain;
		m_Swapchain = LogicalDevice.createSwapchainKHR(SwapchainCreateInfo);
		m_SwapchinCreateInfo = std::move(SwapchainCreateInfo);

		if(Created)
		{
			m_Device.destroySwapchainKHR(m_SwapchinCreateInfo.oldSwapchain);
		}
		m_Device = LogicalDevice;
		Created = true;
	}

	void Swapchain::recreateSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice)
	{
		vk::SwapchainKHR OldSwapchain = m_Swapchain;
		SwapchainCreateInfo.oldSwapchain = OldSwapchain;
		createSwapchain(std::move(SwapchainCreateInfo), LogicalDevice);


	}

	Swapchain::Swapchain(Swapchain&& Other) noexcept :
		m_SwapchinCreateInfo(std::move(Other.m_SwapchinCreateInfo)),
		m_Swapchain(Other.m_Swapchain),
		m_Device(Other.m_Device),
		Created(Other.Created)
	{
		Other.Created = false;
	}

	Swapchain& Swapchain::operator=(Swapchain&& Other) noexcept
	{
		m_SwapchinCreateInfo = std::move(Other.m_SwapchinCreateInfo);
		m_Swapchain = Other.m_Swapchain;
		m_Device = Other.m_Device;
		Created = Other.Created;
		Other.Created = false;

		return *this;
	}
	Swapchain::~Swapchain()
	{
		if (Created) { m_Device.waitIdle(); m_Device.destroySwapchainKHR(m_Swapchain); }
	}

	vk::SurfaceFormatKHR Swapchain::findSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat) const
	{
		if (SupportedSurfaceFormats[0] == PreferredSurfaceFormat[0]) { return SupportedSurfaceFormats[0]; }

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
		if (SupportedPresentMode[0] == PreferredPresentModes[0]) { return SupportedPresentMode[0]; }

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
