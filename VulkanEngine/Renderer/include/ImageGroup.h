#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	class ImageGroup
	{
	public:
		void bindDevice(vk::Device);
		/*
		 * ImageInfo - Image info
		 * ViewCreateInfo - Image view info, .image field will be inserted after successful image creation
		 * LogicalDevice - Device used to create
		 */
		void createImages(std::span<const vk::ImageCreateInfo> ImageInfo);
		void createViews(std::span<vk::ImageViewCreateInfo> ViewCreateInfo);

		ImageGroup() = default;
		ImageGroup(ImageGroup&&) noexcept;
		ImageGroup& operator = (ImageGroup&&) noexcept;

		ImageGroup (const ImageGroup&) = delete;
		ImageGroup operator = (const ImageGroup&) = delete;
		~ImageGroup();

		std::vector<vk::Image> Images;
		std::vector<vk::ImageView> ImageViews;
	private:
		
		vk::Device m_LogicalDevice;


		enum ConstructedObj : uint8_t
		{
			eImage = 0b1,
			eImageView = 0b10
		};
		uint8_t m_Constructed{};
	};
}
