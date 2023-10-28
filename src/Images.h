#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
class Images
{
public:

private:
	std::vector<vk::Image> m_SwapChainImages;
	std::vector<vk::ImageView> m_SwapChainImageViews;
};