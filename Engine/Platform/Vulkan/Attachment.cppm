module;
#include <utility>
#include "Vulkan.h"
export module VT.Platform.Vulkan.Attachment;

import VT.Log;
import VT.Platform.Vulkan.Native.PhysicalDevice;
import VT.Platform.Vulkan.Image;

export namespace VT::Vulkan
{
class FrameBuffer
{
public:
    void Create(vk::Device Device, vk::FramebufferCreateInfo FB_Info)
    {
        m_LogicalDevice   = Device;
        m_FramebufferInfo = FB_Info;

        vk::Result Result;
        std::tie(Result, m_FrameBuffer) = m_LogicalDevice.createFramebuffer(FB_Info);

        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create framebuffer");
    }

    vk::Framebuffer Get() const { return m_FrameBuffer; }

    vk::FramebufferCreateInfo& GetInfo() { return m_FramebufferInfo; }

    void Destroy()
    {
        m_LogicalDevice.destroyFramebuffer(m_FrameBuffer);
        m_FrameBuffer = VK_NULL_HANDLE;
    }

public:
    FrameBuffer() = default;
    FrameBuffer(vk::Device Device, vk::FramebufferCreateInfo FB_Info) { Create(Device, FB_Info); }
    FrameBuffer(const FrameBuffer&)            = delete;
    FrameBuffer& operator=(FrameBuffer& Other) = delete;

    FrameBuffer(FrameBuffer&& Other) :
        m_FramebufferInfo(Other.m_FramebufferInfo),
        m_FrameBuffer(Other.m_FrameBuffer),
        m_LogicalDevice(Other.m_LogicalDevice)
    {
        Other.m_FrameBuffer = VK_NULL_HANDLE;
    }

    FrameBuffer& operator=(FrameBuffer&& Other)
    {
        m_FramebufferInfo   = std::move(Other.m_FramebufferInfo);
        m_FrameBuffer       = std::move(Other.m_FrameBuffer);
        Other.m_FrameBuffer = VK_NULL_HANDLE;
        m_LogicalDevice     = Other.m_LogicalDevice;

        return *this;
    }
    ~FrameBuffer() { Destroy(); }

private:
    vk::FramebufferCreateInfo m_FramebufferInfo;
    vk::Framebuffer m_FrameBuffer;
    vk::Device m_LogicalDevice;
};

class DepthStencil
{
public:
    VulkanImage ImageResource;

    void Create(vk::Extent2D ImageExtent,
                vk::SampleCountFlagBits NumSample,
                vk::Format DepthFormat,
                const vk::PhysicalDeviceMemoryProperties MemProperties,
                vk::Device Device)
    {
        LogicalDevice = Device;

        vk::Extent3D ImageExtent3D {ImageExtent.width, ImageExtent.height, 1};

        vk::ImageCreateInfo ImageInfo {
            .imageType     = vk::ImageType::e2D,
            .format        = DepthFormat,
            .extent        = ImageExtent3D,
            .mipLevels     = 1,
            .arrayLayers   = 1,
            .samples       = NumSample,
            /*
             * Tiling cannot be changed
             * The tiling field can have one of two values:
             * VK_IMAGE_TILING_LINEAR: Texels are laid out in row major order like our pixels array
             * VK_IMAGE_TILING_OPTIMAL : Texels are laid out in an implementation defined order for optimal access
             */
            .tiling        = vk::ImageTiling::eOptimal,
            .usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment,
            // for multisampling
            // optional flags for sparce images, not using
            .sharingMode   = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined};

        ImageResource.CreateImage(ImageInfo, LogicalDevice);
        ImageResource.AllocateMem(MemProperties);
        ImageResource.MemBind(0);

        // ImageView
        vk::ImageViewCreateInfo ImageViewInfo {
            .image    = ImageResource.Image,
            .viewType = vk::ImageViewType::e2D,
            .format   = DepthFormat,
            .subresourceRange {.aspectMask     = vk::ImageAspectFlagBits::eDepth,
                               .baseMipLevel   = 0,
                               .levelCount     = 1,
                               .baseArrayLayer = 0,
                               .layerCount     = 1}
        };
        ImageResource.CreateView(ImageViewInfo);
    }

    void Destroy() { ImageResource.Destroy(); }

public:
    DepthStencil()                                = default;
    DepthStencil(const DepthStencil&)             = delete;
    DepthStencil(const DepthStencil&&)            = delete;
    DepthStencil& operator=(const DepthStencil&)  = delete;
    DepthStencil& operator=(const DepthStencil&&) = delete;
    ~DepthStencil() { Destroy(); }

private:
    vk::Device LogicalDevice;
};
} // namespace VT::Vulkan
