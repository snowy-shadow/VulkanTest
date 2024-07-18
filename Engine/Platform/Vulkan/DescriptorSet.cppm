module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.DescriptorSet;

export namespace VT::Vulkan
{
class DescriptorSet
{
public:
    void Create(uint32_t MaxDescriptorSets, vk::Device LogicalDevice);

    void Bind(vk::CommandBuffer CmdBuffer, vk::PipelineLayout Layout);

    void UploadData(const vk::DescriptorBufferInfo& BufferInfo);

public:
    void Destroy();
    ~DescriptorSet();

public:
    std::vector<vk::DescriptorSet> DescriptorSets;
    vk::DescriptorPool DescriptorPool        = VK_NULL_HANDLE;
    vk::DescriptorSetLayout DescriptorLayout = VK_NULL_HANDLE;

    uint32_t CurrentDescriptorSet = 0;
    uint32_t MaxDescriptorSets;

    vk::Device LogicalDevice;
};
} // namespace VT::Vulkan