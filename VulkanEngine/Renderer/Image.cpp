#include "Image.h"

namespace VT
{
	Image::Image(const vk::ImageCreateInfo& ImageInfo, vk::ImageViewCreateInfo ViewCreateInfo, vk::Device LogicalDevice) : m_Constructed{0}, m_LogicalDevice{std::move(LogicalDevice)}
	{
		m_Image = m_LogicalDevice.createImage(ImageInfo);
		if (!m_Image) { throw std::runtime_error("Failed to create image"); }

		m_Constructed |= ConstructedObj::eImage;
		ViewCreateInfo.image = m_Image;

		m_ImageView = m_LogicalDevice.createImageView(std::move(ViewCreateInfo));
		if(!m_ImageView) { throw std::runtime_error("Failed to create image view"); }
		m_Constructed |= ConstructedObj::eImageView;
	}
	vk::ImageView Image::getImageView() const noexcept
	{
		assert(m_Constructed == ConstructedObj::eImageView);
		return m_ImageView;
	}

	Image::~Image()
	{
		if (m_Constructed & ConstructedObj::eImageView) { m_LogicalDevice.destroyImageView(m_ImageView); }
		if (m_Constructed & ConstructedObj::eImage) { m_LogicalDevice.destroyImage(m_Image); }
	}
}


