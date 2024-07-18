module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Image;

export namespace VT::Vulkan
{
class VulkanImage
{
public:
    void CreateImage(const vk::ImageCreateInfo& ImageInfo, vk::Device Device);
    void CreateView(const vk::ImageViewCreateInfo& ImageViewInfo);

    void AllocateMem(const vk::PhysicalDeviceMemoryProperties& MemProperties);
    void MemBind(uint32_t Offset);

    void TransitionImageLayout(vk::Format Format,
                               vk::ImageLayout OldLayout,
                               vk::ImageLayout NewLayout,
                               uint32_t MipLevels,
                               vk::CommandBuffer CmdBuffer);

    void CopyBufferToImage(vk::Buffer Src, uint32_t Width, uint32_t Height, vk::CommandBuffer CmdBuffer);
    
    void DestroyImage();
    void DestroyView();
    void FreeMem();

public:
    VulkanImage()                                    = default;
    VulkanImage& operator=(const VulkanImage& Other) = delete;
    VulkanImage(const VulkanImage& Other)            = delete;
    void Destroy();
    ~VulkanImage();

public:
    vk::ImageCreateInfo ImageInfo;
    vk::Image Image {VK_NULL_HANDLE};
    vk::ImageView ImageView {VK_NULL_HANDLE};
    vk::DeviceMemory ImageMemory {VK_NULL_HANDLE};

private:
    vk::Device m_LogicalDevice {VK_NULL_HANDLE};
};
}