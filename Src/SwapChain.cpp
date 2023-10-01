#include "SwapChain.h"

namespace VT
{
	void SwapChain::createSwapChain(const PhysicalDevice& PD, 
		const std::vector<vk::SurfaceFormatKHR>& PreferedFormats,
		const std::vector<vk::PresentModeKHR>& PreferedPresentations)
	{
		assert(PD.m_PhysicalDevice && PD.m_Surface);

		auto SurfaceFormatSupported{ PD.m_PhysicalDevice.getSurfaceFormatsKHR(PD.m_Surface) };
		if (!FormatSupported(SurfaceFormatSupported, PreferedFormats)) { throw std::runtime_error("Unable to find SurfaceFormat"); }

		auto PresentationCapabilities{ PD.m_PhysicalDevice.getSurfacePresentModesKHR(PD.m_Surface) };
		if (!PresentModeSupported(PresentationCapabilities, PreferedPresentations)) { throw std::runtime_error("Unable to find Present mode"); }

		auto SurfaceCapabilities{ PD.m_PhysicalDevice.getSurfaceCapabilitiesKHR(PD.m_Surface) };


	}

	void SwapChain::setImageCount(const uint32_t& Amount)
	{
		m_ImageCount = Amount;
	};

	void SwapChain::setWidthHeight(const std::array<uint32_t, 2> XY)
	{
		m_WidthHeight = XY;
	}

	/* ====================================================================
	 *							Private
	 * ====================================================================
	 */

	bool SwapChain::FormatSupported(const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats, const std::vector<vk::SurfaceFormatKHR>& PreferredSurfaceFormat)
	{
		std::unordered_set<vk::SurfaceFormatKHR> SSF{ SupportedSurfaceFormats.cbegin(), SupportedSurfaceFormats.cend() };

		for(const auto& SF : PreferredSurfaceFormat)
		{
			if(SSF.contains(SF))
			{
				m_SurfaceFormat = SF;
				return true;
			}
		}

		return false;
	}

	bool SwapChain::PresentModeSupported(const std::vector<vk::PresentModeKHR>& SupportedPresentation, const std::vector<vk::PresentModeKHR>& PreferredPresentModes)
	{
		std::unordered_set<vk::PresentModeKHR> SPM{ SupportedPresentation.cbegin(), SupportedPresentation.cend() };

		for (const auto& PM : PreferredPresentModes)
		{
			if (SPM.contains(PM))
			{
				m_PresentMode = PM;
				return true;
			}
		}

		return false;
	}
}