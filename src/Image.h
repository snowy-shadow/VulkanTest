#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	class Image
	{
	public:
		void createImageView();
	private:
		vk::Image m_Image;
		vk::ImageView m_ImageView;
		vk::DeviceMemory m_ImageMemory;
	};
}
