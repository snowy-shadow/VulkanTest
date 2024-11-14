module;
#include "Vulkan.h"
#include "stb/stb_image.h"

module VT.Platform.Vulkan.Texture;

import VT.Log;

import VT.Function;

namespace VT::Vulkan
{
VulkanTexture::VulkanTexture(const TextureCreateInfo& TextureInfo,
                             const vk::PhysicalDeviceMemoryProperties& MemProperties,
                             vk::CommandBuffer CmdBuffer,
                             vk::Device Device)
{
    LogicalDevice     = Device;
    /* ============================================
     *            Read Image from file
     * ============================================
     */
    bool FreeImage    = false;
    std::byte* Data   = nullptr;
    uint32_t DataSize = 0;

    if (std::strcmp(TextureInfo.File, "") != 0)
    {
        VT_CORE_ASSERT(TextureInfo.Width < (uint32_t) std::numeric_limits<int>::max() &&
                           TextureInfo.Height < (uint32_t) std::numeric_limits<int>::max() &&
                           TextureInfo.Channels < (uint32_t) std::numeric_limits<int>::max(),
                       "Texture Width, Height or Channel too large");

        // FIX : Only supports RGBA
        int X, Y, C;
        Data = LoadImage(TextureInfo.File, &X, &Y, &C, STBI_rgb_alpha); // alpha padding if no alpha

        Width        = X;
        Height       = Y;
        Channels     = 4;
        Transluscent = (C == STBI_grey_alpha || C == STBI_rgb_alpha);
        DataSize     = Width * Height * 4;
        FreeImage    = true;
    }
    else
    {
        Data         = TextureInfo.pData;
        DataSize     = TextureInfo.DataSize;
        Width        = TextureInfo.Width;
        Height       = TextureInfo.Height;
        Channels     = TextureInfo.Channels;
        Transluscent = TextureInfo.Transluscent;
    }

    /* ============================================
     *            Copy data to image
     * ============================================
     */
    {
        vk::BufferCreateInfo BufferInfo {
            .size        = DataSize,
            .usage       = vk::BufferUsageFlagBits::eTransferSrc,
            .sharingMode = vk::SharingMode::eExclusive,
        };

        ImageBuffer.Create(BufferInfo, LogicalDevice);
        ImageBuffer.BindMem(
            0, MemProperties, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        ImageBuffer.LoadData(Data, DataSize, 0);

        // FIX : Crashes on vk::Format::eR8G8B8Unorm (no alpha)
        vk::ImageCreateInfo ImageInfo {
            .imageType     = vk::ImageType::e2D,
            .format        = vk::Format::eR8G8B8A8Unorm,
            .extent        = vk::Extent3D {Width, Height, 1},
            .mipLevels     = 1,
            .arrayLayers   = 1,
            .samples       = vk::SampleCountFlagBits::e1,
            .tiling        = vk::ImageTiling::eOptimal,
            .usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            .sharingMode   = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined
        };
        Image.CreateImage(ImageInfo, LogicalDevice);
        Image.AllocateMem(MemProperties);
        Image.MemBind(0);
        vk::ImageViewCreateInfo ImageViewInfo {
            .image            = Image.Image,
            .viewType         = vk::ImageViewType::e2D,
            .format           = vk::Format::eR8G8B8A8Unorm,
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
        Image.CopyBufferToImage(ImageBuffer.Buffer, Width, Height, CmdBuffer);
        // Prepare texture for shader access
        Image.TransitionImageLayout(vk::Format::eR8G8B8A8Unorm,
                                    vk::ImageLayout::eTransferDstOptimal,
                                    vk::ImageLayout::eShaderReadOnlyOptimal,
                                    1,
                                    CmdBuffer);
    }
    if (FreeImage)
    {
        UnloadImage(Data);
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
    std::tie(Result, Sampler) = LogicalDevice.createSampler(SamplerInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create sampler");
}

void VulkanTexture::Trim() { ImageBuffer.Destroy(); }

VulkanTexture::~VulkanTexture()
{
    VK_CHECK(LogicalDevice.waitIdle(), vk::Result::eSuccess, "Failed to wait device idle");
    LogicalDevice.destroySampler(Sampler);
}
} // namespace VT::Vulkan
