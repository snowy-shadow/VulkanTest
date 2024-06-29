module;
#include <vulkan/vulkan.hpp>
#include <cstdint>
#include <utility>
#include "EngineMacro.h"
export module VT.Platform.Vulkan.Attachments;

import VT.Log;
import VT.Platform.Vulkan.PhysicalDevice;

#define VK_CHECK(res, expect, ...)              \
    VT_CORE_ASSERT(                             \
        static_cast<vk::Result>(res) == expect, \
        "Vulkan check failed at ",              \
        __FILE__,                               \
        __LINE__,                               \
        " : ",                                  \
        __VA_ARGS__);

export namespace VT::Vulkan
{
class DepthStencil
{
public:
    vk::Image Image;
    vk::ImageView ImageView;
    vk::DeviceMemory ImageMemory;

    void Create(
        vk::Extent2D ImageExtent,
        vk::SampleCountFlagBits NumSample,
        vk::Format DepthFormat,
        vk::Device Device,
        const PhysicalDevice& PD)
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

        Image = LogicalDevice.createImage(ImageInfo);

        // Memory
        const auto MemReq = LogicalDevice.getImageMemoryRequirements(Image);

        const auto [Result, MemIndex] =
            PD.FindMemoryType(MemReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

        VT_CORE_ASSERT(Result, "Failed to find depth memory type");

        vk::MemoryAllocateInfo MemAllocInfo {.allocationSize = MemReq.size, .memoryTypeIndex = MemIndex};

        ImageMemory = LogicalDevice.allocateMemory(MemAllocInfo);

        // Image bind to mem
        LogicalDevice.bindImageMemory(Image, ImageMemory, 0);

        // ImageView
        vk::ImageViewCreateInfo ImageViewInfo {
            .image    = Image,
            .viewType = vk::ImageViewType::e2D,
            .format   = DepthFormat,
            .subresourceRange {
                               .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                               .baseMipLevel   = 0,
                               .levelCount     = 1,
                               .baseArrayLayer = 0,
                               .layerCount     = 1}
        };

        ImageView = LogicalDevice.createImageView(ImageViewInfo);
    }

    void Destroy()
    {
        if (ImageView != VK_NULL_HANDLE)
        {
            LogicalDevice.destroyImageView(ImageView);
        }
        if (Image != VK_NULL_HANDLE)
        {
            LogicalDevice.destroyImage(Image);
        }
        if (ImageMemory != VK_NULL_HANDLE)
        {
            LogicalDevice.freeMemory(ImageMemory);
        }
    }

private:
    vk::Device LogicalDevice;
};
} // namespace VT::Vulkan
