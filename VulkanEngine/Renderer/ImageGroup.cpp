#include "ImageGroup.h"

#include <iostream>

namespace VT
{
	void ImageGroup::bindDevice(vk::Device LogicalDevice)
	{
		m_LogicalDevice = LogicalDevice;
	}

	void ImageGroup::createImages(std::span<const vk::ImageCreateInfo> ImageInfo)
	{
		for(const auto& info : ImageInfo)
		{
			if (!Images.emplace_back(m_LogicalDevice.createImage(info))) 
			{ throw std::runtime_error("Failed to create image"); }
		}

		m_Constructed |= eImage;
	}

	void ImageGroup::createViews(std::span<vk::ImageViewCreateInfo> ViewCreateInfo)
	{
		for (const auto& info : ViewCreateInfo)
		{
			if (!ImageViews.emplace_back(m_LogicalDevice.createImageView(info)))
			{ throw std::runtime_error("Failed to create image view"); }
		}

		m_Constructed |= eImageView;
	}

	ImageGroup::ImageGroup(ImageGroup&& Other) noexcept :
		Images(std::move(Other.Images)),
		ImageViews(std::move(Other.ImageViews)),
		m_LogicalDevice(Other.m_LogicalDevice),
		m_Constructed(Other.m_Constructed)
	{
		Other.m_Constructed = 0;
	}

	ImageGroup::~ImageGroup()
	{
		if(m_Constructed & eImageView)
		{
			for (auto& i : ImageViews)
			{
				m_LogicalDevice.destroyImageView(i);
			}
		}
		if (m_Constructed & eImage)
		{
			for (auto& i : Images)
			{
				m_LogicalDevice.destroyImage(i);
			}
		}
		
	}
}
