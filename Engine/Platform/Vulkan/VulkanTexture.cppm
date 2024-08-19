module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Texture;

import VT.Camera;
import VT.Texture;
import VT.Platform.Vulkan.Image;
import VT.Platform.Vulkan.Buffer;

export namespace VT::Vulkan
{
class VulkanTexture : public Texture
{
public:
    /**
     * Create vulkan texture
     * @param TextureInfo Texture info
     * @param MemProperties Physical device memory properties
     * @param CmdBuffer Buffer used to copy data to image
     * @param Device Logical device
     */
    VulkanTexture(const TextureCreateInfo& TextureInfo,
                  const vk::PhysicalDeviceMemoryProperties& MemProperties,
                  vk::CommandBuffer CmdBuffer,
                  vk::Device Device);

    // Free redundant resources after constructor
    void Trim();

    virtual ~VulkanTexture() override;

public:
    VulkanImage Image;
    vk::Sampler Sampler;

    // FIX : Remove hack
    VulkanBuffer ImageBuffer;

    // Handle
    vk::Device LogicalDevice;
};
} // namespace VT::Vulkan