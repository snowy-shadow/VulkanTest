module;
#include "Vulkan.h"
module VT.Platform.Vulkan.Texture;

import VT.Platform.Vulkan.Buffer;
import VT.Log;

namespace VT::Vulkan
{
VulkanTexture::VulkanTexture(const TextureCreateInfo& TextureInfo,
                           const vk::PhysicalDeviceMemoryProperties& MemProperties,
                           vk::CommandBuffer CmdBuffer,
                           vk::Device Device)
{
    this->Width        = TextureInfo.Width;
    this->Height       = TextureInfo.Height;
    this->ChannelCount = TextureInfo.ChannelCount;
    this->Generation   = TextureInvalideID;

    /* ============================================
     *            Copy data to image
     * ============================================
     */
    {
        vk::BufferCreateInfo BufferInfo {
            .size        = TextureInfo.Width * TextureInfo.Height * TextureInfo.ChannelCount,
            .usage       = vk::BufferUsageFlagBits::eTransferSrc,
            .sharingMode = vk::SharingMode::eExclusive,
        };

        VulkanBuffer StagingBuffer;
        StagingBuffer.Create(BufferInfo, Device);
        StagingBuffer.BindMem(
            0, MemProperties, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        StagingBuffer.LoadData(TextureInfo.pData, TextureInfo.DataSize, 0);

        vk::ImageCreateInfo ImageInfo {
            .imageType     = vk::ImageType::e2D,
            .format        = vk::Format::eR8G8B8A8Unorm,
            .extent        = vk::Extent3D {TextureInfo.Width, TextureInfo.Height, 1},
            .mipLevels     = 1,
            .arrayLayers   = 1,
            .samples       = vk::SampleCountFlagBits::e1,
            .tiling        = vk::ImageTiling::eOptimal,
            .usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            .sharingMode   = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined
        };
        Image.CreateImage(ImageInfo, Device);

        vk::ImageViewCreateInfo ImageViewInfo {
            .image    = Image.Image,
            .viewType = vk::ImageViewType::e2D,
            .format   = vk::Format::eR8G8B8A8Unorm,
            .subresourceRange = {.aspectMask     = vk::ImageAspectFlagBits::eColor,
                                 .baseMipLevel   = 0,
                                 .levelCount     = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount     = 1}
        };
        Image.CreateView(ImageViewInfo);

        Image.TransitionImageLayout(vk::Format::eR8G8B8A8Unorm,
                                    vk::ImageLayout::eUndefined,
                                    vk::ImageLayout::eTransferDstOptimal,
                                    1,
                                    CmdBuffer);
        Image.CopyBufferToImage(StagingBuffer.Buffer, TextureInfo.Width, TextureInfo.Height, CmdBuffer);
    }

    /* ============================================
     *                  Sampler
     * ============================================
     */
    vk::SamplerCreateInfo SamplerInfo {
        .magFilter               = vk::Filter::eLinear,
        .minFilter               = vk::Filter::eLinear,
        .mipmapMode              = vk::SamplerMipmapMode::eLinear,
        .addressModeU            = vk::SamplerAddressMode::eRepeat,
        .addressModeV            = vk::SamplerAddressMode::eRepeat,
        .addressModeW            = vk::SamplerAddressMode::eRepeat,
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = vk::True,
        .maxAnisotropy           = 16,
        .compareEnable           = vk::False,
        .compareOp               = vk::CompareOp::eAlways,
        .minLod                  = 0.0f,
        .maxLod                  = 0.0f,
        .borderColor             = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = vk::False,
    };
    vk::Result Result;
    std::tie(Result, Sampler) = Device.createSampler(SamplerInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create sampler");

    this->Transparent = TextureInfo.Transparent;
    this->Generation++;
}

VulkanTexture::~VulkanTexture()
{
    VK_CHECK(LogicalDevice.waitIdle(), vk::Result::eSuccess, "Failed to wait device idle");
    Image.Destroy();
    LogicalDevice.destroySampler(Sampler);
}
} // namespace VT::Vulkan