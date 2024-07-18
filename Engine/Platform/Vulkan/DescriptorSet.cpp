module;
#include "Vulkan.h"
#include <glm/glm.hpp>
module VT.Platform.Vulkan.DescriptorSet;
import VT.Log;

namespace VT::Vulkan
{
void DescriptorSet::Create(uint32_t MaxDescriptorSets, vk::Device Device)
{
    /* ===================================================================
     *                    Descriptor set
     * ===================================================================
     */
    {
        this->LogicalDevice     = Device;
        this->MaxDescriptorSets = MaxDescriptorSets;

        vk::DescriptorSetLayoutBinding DescriptorLayoutBinding {
            .binding            = 0,
            .descriptorType     = vk::DescriptorType::eUniformBuffer,
            .descriptorCount    = 1,
            .stageFlags         = vk::ShaderStageFlagBits::eVertex,
            .pImmutableSamplers = nullptr,
        };

        vk::DescriptorSetLayoutCreateInfo DescriptorLayoutInfo {
            .bindingCount = 1,
            .pBindings    = &DescriptorLayoutBinding,
        };

        vk::Result Result;
        std::tie(Result, DescriptorLayout) = LogicalDevice.createDescriptorSetLayout(DescriptorLayoutInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor set layout");

        vk::DescriptorPoolSize PoolSize {
            .type            = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = MaxDescriptorSets,
        };

        vk::DescriptorPoolCreateInfo PoolInfo {
            .maxSets       = MaxDescriptorSets,
            .poolSizeCount = 1,
            .pPoolSizes    = &PoolSize,
        };

        std::tie(Result, DescriptorPool) = LogicalDevice.createDescriptorPool(PoolInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor pool");

        // Allocate descriptor set
        vk::DescriptorSetLayout pDescriptorSetLayout[] {DescriptorLayout, DescriptorLayout, DescriptorLayout};
        vk::DescriptorSetAllocateInfo DescriptorSetAllocInfo {
            .descriptorPool     = DescriptorPool,
            .descriptorSetCount = MaxDescriptorSets,
            .pSetLayouts        = pDescriptorSetLayout,
        };

        std::tie(Result, DescriptorSets) = LogicalDevice.allocateDescriptorSets(DescriptorSetAllocInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to Allocate descriptor sets");
    }
}
void DescriptorSet::Bind(vk::CommandBuffer CmdBuffer, vk::PipelineLayout Layout)
{
    CmdBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, Layout, 0, 1, &DescriptorSets[CurrentDescriptorSet], 0, nullptr);
}
void DescriptorSet::UploadData(const vk::DescriptorBufferInfo& BufferInfo)
{
    CurrentDescriptorSet = (CurrentDescriptorSet + 1) % MaxDescriptorSets;

    // VT_CORE_INFO("Projection {}\n View {}", glm::to_string(Data.ProjectionMatrix), glm::to_string(Data.ViewMatrix));

    vk::WriteDescriptorSet WriteDescriptorSet {
        .dstSet          = DescriptorSets[CurrentDescriptorSet],
        .dstBinding      = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &BufferInfo,
    };

    LogicalDevice.updateDescriptorSets(1, &WriteDescriptorSet, 0, nullptr);
}
void DescriptorSet::Destroy()
{
    VK_CHECK(LogicalDevice.waitIdle(), vk::Result::eSuccess, "Failed to wait logical device idle");
    LogicalDevice.destroyDescriptorSetLayout(DescriptorLayout);
    LogicalDevice.destroyDescriptorPool(DescriptorPool);
    DescriptorPool   = VK_NULL_HANDLE;
    DescriptorLayout = VK_NULL_HANDLE;
}
DescriptorSet::~DescriptorSet() { Destroy(); }
} // namespace VT::Vulkan