module;
#include <vulkan/vulkan_hash.hpp>
#include <unordered_set>
#include <algorithm>
#include <utility>

#include "Platform/Vulkan/Vulkan.h"
module VT.Platform.Vulkan.Native.Swapchain;

import VT.Log;

namespace VT::Vulkan::Native
{
Swapchain::Swapchain(vk::Device Device) : m_LogicalDevice(Device) {}

void Swapchain::Init(vk::Device Device) { m_LogicalDevice = Device; }

std::pair<bool, vk::SurfaceFormatKHR> Swapchain::FindSurfaceFormat(
	const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat,
	const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats) const
{
	std::unordered_set<vk::SurfaceFormatKHR> SSF{ SupportedSurfaceFormats.cbegin(), SupportedSurfaceFormats.cend() };

	for (const auto& SF : PreferredSurfaceFormat)
	{
		if (SSF.contains(SF))
		{
			return { true, SF };
		}
	}

	return { false, {} };
}

std::pair<bool, vk::PresentModeKHR> Swapchain::FindPresentMode(
	const std::vector<vk::PresentModeKHR>& PreferredPresentModes,
	const std::vector<vk::PresentModeKHR>& SupportedPresentMode) const
{
	std::unordered_set<vk::PresentModeKHR> SPM{ SupportedPresentMode.cbegin(), SupportedPresentMode.cend() };

	for (const auto& PM : PreferredPresentModes)
	{
		if (SPM.contains(PM))
		{
			return { true, PM };
		}
	}

	return { false, {} };
}

std::pair<bool, vk::SwapchainCreateInfoKHR> Swapchain::QueryCapabilities(
	vk::SwapchainCreateInfoKHR Info,
	Capabilities PreferredCapabilities,
	vk::PhysicalDevice PhysicalDevice,
	vk::SurfaceKHR Surface) const
{
	Info.surface = Surface;

	// image count
	const auto [Result, DeviceSurfaceCapabilities] = PhysicalDevice.getSurfaceCapabilitiesKHR(Surface);
	VK_CHECK(Result, vk::Result::eSuccess, "Failed to get surface capabilities");

	// max Image count = 0 means no upper bound
	if (DeviceSurfaceCapabilities.maxImageCount > 0)
	{
		Info.minImageCount = std::clamp(
			PreferredCapabilities.minImageCount,
			DeviceSurfaceCapabilities.minImageCount,
			DeviceSurfaceCapabilities.maxImageCount);
	}
	else if (PreferredCapabilities.minImageCount < DeviceSurfaceCapabilities.minImageCount)
	{
		Info.minImageCount = DeviceSurfaceCapabilities.minImageCount;
	}

	// Image extent
	// undefined surface size, set it to image size requested
	if (DeviceSurfaceCapabilities.currentExtent.width == std::numeric_limits<std::uint32_t>::max())
	{
		Info.imageExtent = {
			std::clamp(
				PreferredCapabilities.imageExtent.width,
				DeviceSurfaceCapabilities.minImageExtent.width,
				DeviceSurfaceCapabilities.maxImageExtent.width),
			std::clamp(
				PreferredCapabilities.imageExtent.height,
				DeviceSurfaceCapabilities.minImageExtent.height,
				DeviceSurfaceCapabilities.maxImageExtent.height)
		};
	}
	else
	{
		Info.imageExtent = DeviceSurfaceCapabilities.currentExtent;
	}

	// Array layers
	Info.imageArrayLayers = std::clamp(
		PreferredCapabilities.arrayLayers,
		PreferredCapabilities.arrayLayers,
		DeviceSurfaceCapabilities.maxImageArrayLayers);

	// Composite alpha
	{
		const auto Iterator{
			std::find_if(
				PreferredCapabilities.compositeAlpha.cbegin(),
				PreferredCapabilities.compositeAlpha.cend(),
				[&DeviceSurfaceCapabilities](const vk::CompositeAlphaFlagBitsKHR& PCA)
				{
					return DeviceSurfaceCapabilities.supportedCompositeAlpha & PCA;
				})
		};

		if (Iterator == PreferredCapabilities.compositeAlpha.cend())
		{
			VT_CORE_ERROR("Did not find required composite alpha");
			return { false, Info };
		}

		Info.compositeAlpha = PreferredCapabilities.compositeAlpha[static_cast<uint32_t>(
			std::distance(PreferredCapabilities.compositeAlpha.cbegin(), Iterator))];
	}

	// Surface transform
	{
		const auto Iterator{
			std::find_if(
				PreferredCapabilities.surfaceTransform.cbegin(),
				PreferredCapabilities.surfaceTransform.cend(),
				[&DeviceSurfaceCapabilities](const vk::SurfaceTransformFlagBitsKHR& ST)
				{
					return DeviceSurfaceCapabilities.supportedTransforms & ST;
				})
		};

		if (Iterator == PreferredCapabilities.surfaceTransform.cend())
		{
			VT_CORE_ERROR("Did not find required Surface Transform");
			return { false, Info };
		}

		Info.preTransform = PreferredCapabilities.surfaceTransform[static_cast<uint32_t>(
			std::distance(PreferredCapabilities.surfaceTransform.cbegin(), Iterator))];
	}

	return { true, Info };
}

void Swapchain::CreateSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice)
{
	SwapchainCreateInfo.oldSwapchain = m_Swapchain;
	vk::Result Result;
	std::tie(Result, m_Swapchain) = LogicalDevice.createSwapchainKHR(SwapchainCreateInfo);
	VK_CHECK(Result, vk::Result::eSuccess, "Failed to create Swapchain");

	m_SwapchainCreateInfo = std::move(SwapchainCreateInfo);

	if (m_SwapchainCreated)
	{
		Result = m_LogicalDevice.waitIdle();
		VK_CHECK(Result, vk::Result::eSuccess, "Failed to wait for logical device");

		m_LogicalDevice.destroySwapchainKHR(m_SwapchainCreateInfo.oldSwapchain);
	}
	m_LogicalDevice    = LogicalDevice;
	m_SwapchainCreated = true;
}

void Swapchain::RecreateSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice)
{
	vk::SwapchainKHR OldSwapchain    = m_Swapchain;
	SwapchainCreateInfo.oldSwapchain = OldSwapchain;
	CreateSwapchain(std::move(SwapchainCreateInfo), LogicalDevice);
}

vk::SwapchainKHR Swapchain::Get() const noexcept { return m_Swapchain; }

vk::SwapchainCreateInfoKHR Swapchain::GetInfo() const noexcept { return m_SwapchainCreateInfo; }

Swapchain::Swapchain(Swapchain&& Other) noexcept :
	m_LogicalDevice(Other.m_LogicalDevice),
	m_SwapchainCreateInfo(std::move(Other.m_SwapchainCreateInfo)),
	m_Swapchain(Other.m_Swapchain),
	m_SwapchainCreated(Other.m_SwapchainCreated)

{
	Other.m_SwapchainCreated = false;
}

Swapchain& Swapchain::operator=(Swapchain&& Other) noexcept
{
	m_SwapchainCreateInfo    = std::move(Other.m_SwapchainCreateInfo);
	m_Swapchain              = Other.m_Swapchain;
	m_LogicalDevice          = Other.m_LogicalDevice;
	m_SwapchainCreated       = Other.m_SwapchainCreated;
	Other.m_SwapchainCreated = false;

	return *this;
}

void Swapchain::Destroy()
{
	if (m_SwapchainCreated)
	{
		const auto Result = m_LogicalDevice.waitIdle();
		VK_CHECK(Result, vk::Result::eSuccess, "Failed to wait for logical device");

		m_LogicalDevice.destroySwapchainKHR(m_Swapchain);
	}
	m_SwapchainCreated = false;
}

Swapchain::~Swapchain() { Destroy(); }
} // namespace VT::Vulkan::Native
