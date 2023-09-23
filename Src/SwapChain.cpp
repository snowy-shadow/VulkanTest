#include "SwapChain.h"

namespace VT
{

	void SwapChain::createSwapChain(const PhysicalDevice& PD)
	{
		auto SurfaceFormat{ PD.m_PhysicalDevice.getSurfaceFormatsKHR(PD.m_Surface) };

		auto Result = std::find_if(SurfaceFormat.cbegin(), SurfaceFormat.cend(), 
			[&](vk::SurfaceFormatKHR SF) {return SF.format == m_SurfaceFormat; });
		if (Result == SurfaceFormat.cend()) { throw std::runtime_error("Did not find the required surface format"); }

		auto SurfaceCapabilities{ PD.m_PhysicalDevice.getSurfaceCapabilitiesKHR(PD.m_Surface) };

	}

	void SwapChain::setImageFormat(const vk::Format& Format)
	{
		m_SurfaceFormat = Format;
	};

	void SwapChain::setImageCount(const uint32_t& Amount)
	{
		m_ImageCount = Amount;
	};

	void SwapChain::setMaxWidthHeight(const uint32_t& Width, const uint32_t& Height)
	{
		m_Width = Width;
		m_Height = Height;
	};
}