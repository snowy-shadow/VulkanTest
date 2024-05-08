#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	class Image
	{
	public:
		/*
		 * ImageInfo - Image info
		 * ViewCreateInfo - Image view info, .image field will be inserted after successful image creation
		 * LogicalDevice - Device used to create
		 */
		void create(const vk::ImageCreateInfo& ImageInfo, vk::ImageViewCreateInfo ViewCreateInfo, vk::Device LogicalDevice);
		~Image();

		vk::ImageView getImageView() const noexcept;
	private:
		vk::Image m_Image;
		vk::ImageView m_ImageView;
		vk::Device m_LogicalDevice;

		enum ConstructedObj : uint8_t
		{
			eImage = 0b1,
			eImageView = 0b10
		};
		uint8_t m_Constructed{};
	};
}
