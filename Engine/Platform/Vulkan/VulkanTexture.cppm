module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Texture;

import VT.Texture;
import VT.Platform.Vulkan.Image;

export namespace VT::Vulkan
{
static constexpr uint32_t TextureInvalideID = -1;

struct VulkanTexture : VT::Texture
{
    /**
     * Create vulkan texture
     * @param TextureInfo Texture info
     * @param MemProperties Physical device memory properties
     * @param CmdBuffer Buffer used to copy data to image
     * @param Device Logical device
     */
    void Create(const TextureCreateInfo& TextureInfo,
                const vk::PhysicalDeviceMemoryProperties& MemProperties,
                vk::CommandBuffer CmdBuffer,
                vk::Device Device);
    void Destroy();

public:
    ~VulkanTexture();

public:
    VulkanImage Image;
    vk::Sampler Sampler;
    vk::Device LogicalDevice;
};
} // namespace VT::Vulkan