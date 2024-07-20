module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Texture;

import VT.RendererType;
import VT.Platform.Vulkan.Image;
import VT.Camera;

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

    virtual ~VulkanTexture() override;

public:
    VulkanImage Image;
    vk::Sampler Sampler;
    vk::Device LogicalDevice;
};
} // namespace VT::Vulkan