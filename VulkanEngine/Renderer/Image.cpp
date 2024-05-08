#include "Image.h"

namespace VT
{
	void Image::create(const vk::ImageCreateInfo& ImageInfo, vk::ImageViewCreateInfo ViewCreateInfo, vk::Device LogicalDevice)
	{
		m_LogicalDevice = std::move(LogicalDevice);

		m_Image = m_LogicalDevice.createImage(ImageInfo);
		if (!m_Image) { throw std::runtime_error("Failed to create image"); }

		m_Constructed |= eImage;
		ViewCreateInfo.image = m_Image;

		m_ImageView = m_LogicalDevice.createImageView(ViewCreateInfo);
		if(!m_ImageView) { throw std::runtime_error("Failed to create image view"); }
		m_Constructed |= eImageView;
	}
	vk::ImageView Image::getImageView() const noexcept
	{
		assert(m_Constructed == eImageView);
		return m_ImageView;
	}

	Image::~Image()
	{
		if (m_Constructed & eImageView) { m_LogicalDevice.destroyImageView(m_ImageView); }
		if (m_Constructed & eImage) { m_LogicalDevice.destroyImage(m_Image); }
	}
}


