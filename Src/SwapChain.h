#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		void setSwapChainExtent(vk::Extent2D Extent);

	private:
		vk::SwapchainKHR m_SwapChain;
	};
}
