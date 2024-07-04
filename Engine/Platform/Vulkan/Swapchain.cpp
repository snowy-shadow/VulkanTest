module;
#include <limits>
#include "Vulkan.h"
module VT.Platform.Vulkan.Swapchain;

import VT.Log;

namespace VT::Vulkan
{
void Swapchain::Init(
	Window& Window,
	vk::Device LogicalDevice,
	Native::PhysicalDevice& PhysicalDevice,
	vk::SurfaceKHR Surface,
	uint32_t MaxFrameCount)
{
	m_PhysicalDevice = &PhysicalDevice;
	m_LogicalDevice  = LogicalDevice;
	m_Surface        = Surface;

    /* ===============================================
     *          Create Swapchain
     * ===============================================
     */
	{
		m_Swapchain.Init(m_LogicalDevice);

		vk::SwapchainCreateInfoKHR SwapchainInfo{ .imageUsage = vk::ImageUsageFlagBits::eColorAttachment };

		{
			const auto [R, SF] = m_PhysicalDevice->Get().getSurfaceFormatsKHR(m_Surface);

			VK_CHECK(R, vk::Result::eSuccess, "Failed to query surface format");

			const auto [Result, Surfaceformat] =
				m_Swapchain.FindSurfaceFormat(
					{ { { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear } } },
					SF);

			VT_CORE_ASSERT(Result, "Could not find required surface format");
			SwapchainInfo.imageFormat     = Surfaceformat.format;
			SwapchainInfo.imageColorSpace = Surfaceformat.colorSpace;
		}

		{
			const auto [R, PM] = m_PhysicalDevice->Get().getSurfacePresentModesKHR(m_Surface);
			VK_CHECK(R, vk::Result::eSuccess, "Failed to query present mode");

			const auto [Result, PresentMode] = m_Swapchain.FindPresentMode({ vk::PresentModeKHR::eFifo }, PM);

			VT_CORE_ASSERT(Result, "Could not find required present mode");
			SwapchainInfo.presentMode = PresentMode;
		}

		Native::Swapchain::Capabilities SwapchainQueries{
			.minImageCount = MaxFrameCount,
            .imageExtent = { Window.GetWidth(), Window.GetHeight() },
			.arrayLayers = 1,
			.surfaceTransform = { vk::SurfaceTransformFlagBitsKHR::eIdentity },
			.compositeAlpha = { vk::CompositeAlphaFlagBitsKHR::eOpaque },
		};

		auto [Result, SwapchainCreateInfo] =
			m_Swapchain.QueryCapabilities(SwapchainInfo, SwapchainQueries, PhysicalDevice.Get(), m_Surface);

		VT_CORE_ASSERT(Result, "Failed to find appropriate swapchain settings");

		if (!m_PhysicalDevice->GraphicsQueueCanPresent())
		{
			uint32_t QueueFamilyIndices[]{
				m_PhysicalDevice->GetGraphicsQueue().queueFamilyIndex,
				m_PhysicalDevice->GetPresentQueue().queueFamilyIndex
			};

			SwapchainCreateInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
			SwapchainCreateInfo.queueFamilyIndexCount = 2;
			SwapchainCreateInfo.pQueueFamilyIndices   = QueueFamilyIndices;
		}

		m_Swapchain.CreateSwapchain(SwapchainCreateInfo, m_LogicalDevice);
	}
	VT_CORE_TRACE("Vulkan swapchain created");

    CreateResources();
}

std::pair<bool, uint32_t> Swapchain::AcquireNextImage(vk::Semaphore Semaphore)
{
	const auto [Result, ImageIndex] = m_LogicalDevice.acquireNextImageKHR(
		m_Swapchain.Get(),
		std::numeric_limits<uint64_t>::max(),
		Semaphore,
		VK_NULL_HANDLE);

    m_CurrentImageIndex = ImageIndex;
    return { Result == vk::Result::eSuccess, ImageIndex };
}

vk::SwapchainKHR Swapchain::Get() const { return m_Swapchain.Get(); }
vk::SwapchainCreateInfoKHR Swapchain::GetInfo() const { return m_Swapchain.GetInfo(); }
uint32_t Swapchain::GetCurrentImageIndex() const { return m_CurrentImageIndex; }

std::vector<std::vector<vk::ImageView>> Swapchain::GetImageView() const
{
    std::vector<std::vector<vk::ImageView>> ImageView(m_ImageView.size());
    for (size_t i = 0; i < m_ImageView.size(); i++)
    {
        ImageView[i] = { m_ImageView[i], m_DepthStencil.ImageView };
    }

    return ImageView;
}

uint32_t Swapchain::GetMaxFrameCount() const { return m_Swapchain.GetInfo().minImageCount; }

void Swapchain::Resize(uint32_t Width, uint32_t Height)
{
	auto Info = m_Swapchain.GetInfo();

	Info.imageExtent = { Width, Height };

	// update surface capabilities, else it will complain
	// cast to void, ignore output - warn unused variable
	(void)m_PhysicalDevice->Get().getSurfaceCapabilitiesKHR(m_Surface);

	m_Swapchain.RecreateSwapchain(Info, m_LogicalDevice);

    DestroyResources();
    CreateResources();
}

Swapchain::~Swapchain()
{
    DestroyResources();
}

/** ===============================================
 *					Private
 *	===============================================
 */
void Swapchain::CreateResources()
{

    /* ===============================================
     *          Depth Image
     * ===============================================
     */
    {
        auto [Result, DepthFormat] = m_PhysicalDevice->FindSupportedFormat(
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);

        VT_CORE_ASSERT(Result, "Failed to find depth format");

        m_DepthStencil.Create(
            m_Swapchain.GetInfo().imageExtent,
            vk::SampleCountFlagBits::e1,
            DepthFormat,
            m_PhysicalDevice->Get().getMemoryProperties(),
            m_LogicalDevice);
    }
    VT_CORE_TRACE("Depth resource created");
    /* ===============================================
     *          Image View
     * ===============================================
     */
    {
        auto [Result, Images] = m_LogicalDevice.getSwapchainImagesKHR(m_Swapchain.Get());
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to get swapchain images");

        m_ImageView.resize(Images.size());

        vk::ImageViewCreateInfo ImageViewInfo{
            .viewType = vk::ImageViewType::e2D,
            .format = m_Swapchain.GetInfo().imageFormat,
            .components = { vk::ComponentSwizzle::eIdentity },
            .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        for (size_t i = 0; i < Images.size(); i++)
        {
            ImageViewInfo.image            = Images[i];
            const auto [Result, ImageView] = m_LogicalDevice.createImageView(ImageViewInfo);

            VK_CHECK(Result, vk::Result::eSuccess, "Failed to create image view {}", std::to_string(i));
            m_ImageView[i] = ImageView;
        }
    }
    m_Initalized = true;
}

void Swapchain::DestroyResources()
{
    if (!m_Initalized) { return; }
    m_DepthStencil.Destroy();

    for (const auto& ImageView : m_ImageView)
    {
        m_LogicalDevice.destroyImageView(ImageView);
    }

    m_Initalized = false;
}
} // namespace VT::Vulkan
