module;
#include "Vulkan.h"
module VT.Platform.Vulkan.Image;

import VT.Log;
import VT.Platform.Vulkan.Util;

namespace VT::Vulkan
{
void VulkanImage::CreateImage(const vk::ImageCreateInfo& ImageInfo, vk::Device Device)
{
    m_LogicalDevice = Device;

    vk::Result Result;
    std::tie(Result, Image) = m_LogicalDevice.createImage(ImageInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create image");
    this->ImageInfo = ImageInfo;
}

void VulkanImage::CreateView(const vk::ImageViewCreateInfo& ImageViewInfo)
{
    VT_CORE_ASSERT(Image != VK_NULL_HANDLE, "No image for image view")
    vk::Result Result;
    std::tie(Result, ImageView) = m_LogicalDevice.createImageView(ImageViewInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create depth image view");
}

void VulkanImage::AllocateMem(const vk::PhysicalDeviceMemoryProperties& MemProperties)
{
    // Memory
    const auto MemReq = m_LogicalDevice.getImageMemoryRequirements(Image);

    const auto [Found, MemIndex] =
        FindMemoryTypeIndex(MemProperties, MemReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    VT_CORE_ASSERT(Found, "Failed to find image memory type");

    vk::MemoryAllocateInfo MemAllocInfo {.allocationSize = MemReq.size, .memoryTypeIndex = MemIndex};

    vk::Result Result;
    std::tie(Result, ImageMemory) = m_LogicalDevice.allocateMemory(MemAllocInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate image memory");
}

void VulkanImage::MemBind(uint32_t Offset)
{
    VT_CORE_ASSERT(Image != VK_NULL_HANDLE, "No image to bind")
    VT_CORE_ASSERT(ImageMemory != VK_NULL_HANDLE, "Memory not allocated")

    // Image bind to mem
    VK_CHECK(m_LogicalDevice.bindImageMemory(Image, ImageMemory, Offset),
             vk::Result::eSuccess,
             "Failed to bind image memory");
}

void VulkanImage::TransitionImageLayout(vk::Format Format,
                                        vk::ImageLayout OldLayout,
                                        vk::ImageLayout NewLayout,
                                        uint32_t MipLevels,
                                        vk::CommandBuffer CmdBuffer)
{
    // use to ensure writes are compelete before reading
    // can also be used to transition image layouts and transfer queue family ownership when VK_SHARING_MODE_EXCLUSIVE
    // is used
    vk::ImageMemoryBarrier MemBarrier {};
    MemBarrier.oldLayout                       = OldLayout;
    MemBarrier.newLayout                       = NewLayout;
    // transfer queue family ownership by giving its indicies, use IGNORED if not transfering
    MemBarrier.srcQueueFamilyIndex             = vk::QueueFamilyIgnored;
    MemBarrier.dstQueueFamilyIndex             = vk::QueueFamilyIgnored;
    MemBarrier.image                           = Image;
    MemBarrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    MemBarrier.subresourceRange.baseMipLevel   = 0;
    MemBarrier.subresourceRange.levelCount     = MipLevels;
    MemBarrier.subresourceRange.baseArrayLayer = 0;
    MemBarrier.subresourceRange.layerCount     = 1;

    /*
        MemBarrier.srcAccessMask = 0;
        MemBarrier.dstAccessMask = 0;
    */

    vk::PipelineStageFlags SourceStage;
    vk::PipelineStageFlags DstStage;

    if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        MemBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
        MemBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        SourceStage = vk::PipelineStageFlagBits::eAllCommands;
        DstStage    = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (OldLayout == vk::ImageLayout::eTransferDstOptimal && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        MemBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        MemBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        SourceStage = vk::PipelineStageFlagBits::eTransfer;
        DstStage    = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        VT_HALT("Unsupported image layout transition");
    }

    CmdBuffer.pipelineBarrier(SourceStage, DstStage, vk::DependencyFlags(), nullptr, nullptr, MemBarrier);
}

void VulkanImage::CopyBufferToImage(vk::Buffer Src, uint32_t Width, uint32_t Height, vk::CommandBuffer CmdBuffer)
{
    VT_CORE_ASSERT(Image != VK_NULL_HANDLE, "No image to copy to")
    VT_CORE_ASSERT(ImageMemory != VK_NULL_HANDLE, "Memory not allocated")

    vk::BufferImageCopy Region {};
    // byte offset in buffer where pixels start
    Region.bufferOffset      = 0;
    Region.bufferRowLength   = 0;
    Region.bufferImageHeight = 0;

    Region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
    Region.imageSubresource.mipLevel       = 0;
    Region.imageSubresource.baseArrayLayer = 0;
    Region.imageSubresource.layerCount     = 1;

    Region.imageOffset = {0, 0, 0};
    Region.imageExtent = {Width, Height, 1};

    // enqueue copy operation
    CmdBuffer.copyBufferToImage(Src,
                                Image,
                                // image layout
                                vk::ImageLayout::eTransferDstOptimal,
                                Region);
}

void VulkanImage::DestroyImage()
{
    m_LogicalDevice.destroyImage(Image);
    Image = VK_NULL_HANDLE;
}

void VulkanImage::DestroyView()
{
    m_LogicalDevice.destroyImageView(ImageView);
    ImageView = VK_NULL_HANDLE;
}
void VulkanImage::FreeMem()
{
    m_LogicalDevice.freeMemory(ImageMemory);
    ImageMemory = VK_NULL_HANDLE;
}

void VulkanImage::Destroy()
{
    VK_CHECK(m_LogicalDevice.waitIdle(), vk::Result::eSuccess, "Failed to wait device idle");
    DestroyView();
    DestroyImage();
    FreeMem();
}

VulkanImage ::~VulkanImage() { Destroy(); };
} // namespace VT::Vulkan
