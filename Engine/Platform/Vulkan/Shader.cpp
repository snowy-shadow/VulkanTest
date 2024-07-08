module;
#include "Vulkan.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
module VT.Platform.Vulkan.Shader;
import VT.Log;

namespace VT::Vulkan
{
void Shader::Create(
    std::span<const HLSL::ShaderFileInfo> Shaders,
    uint32_t MaxDescriptorSets,
    BufferLayout UniformBufferLayout,
    BufferLayout VertexBufferLayout,
    vk::RenderPass Renderpass,
    vk::PhysicalDevice PhysicalDevice,
    vk::Device LogicalDevice)
{
    m_LogicalDevice = LogicalDevice;
    // Descriptor set
    {
        m_MaxDescriptorSets = MaxDescriptorSets;

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
        std::tie(Result, m_DescriptorLayout) = LogicalDevice.createDescriptorSetLayout(DescriptorLayoutInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor set layout");


        vk::DescriptorPoolSize PoolSize {
            .type            = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = m_MaxDescriptorSets,
        };

        vk::DescriptorPoolCreateInfo PoolInfo {
            .maxSets       = m_MaxDescriptorSets,
            .poolSizeCount = 1,
            .pPoolSizes    = &PoolSize,
        };

        std::tie(Result, m_DescriptorPool) = LogicalDevice.createDescriptorPool(PoolInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor pool");

        m_UBO_Buffer.Create(
            {.size  = UniformBufferLayout.GetStride(),
             .usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst},
            LogicalDevice);

        m_UBO_Buffer.BindMem(
            0,
            PhysicalDevice.getMemoryProperties(),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent |
                vk::MemoryPropertyFlagBits::eDeviceLocal);

        // Allocate descriptor set
        vk::DescriptorSetLayout pDescriptorSetLayout[] {m_DescriptorLayout, m_DescriptorLayout, m_DescriptorLayout};
        vk::DescriptorSetAllocateInfo DescriptorSetAllocInfo {
            .descriptorPool     = m_DescriptorPool,
            .descriptorSetCount = m_MaxDescriptorSets,
            .pSetLayouts        = pDescriptorSetLayout,
        };

        std::tie(Result, m_DescriptorSet) = LogicalDevice.allocateDescriptorSets(DescriptorSetAllocInfo);
    }
    VT_CORE_TRACE("Descriptor set created");

    vk::PipelineLayoutCreateInfo PipelineLayout {
        .setLayoutCount         = 1,
        .pSetLayouts            = &m_DescriptorLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr,
    };

    // Shader modules
    std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfo(Shaders.size());

    {
        HLSL::Compiler ShaderCompiler;

        for (std::size_t i = 0; i < Shaders.size(); i++)
        {
            auto Spv = ShaderCompiler.CompileSpv(Shaders[i]);

            const auto [Result, Module] = LogicalDevice.createShaderModule(
                {.codeSize = Spv.size(), .pCode = reinterpret_cast<uint32_t*>(Spv.data())});
            VK_CHECK(Result, vk::Result::eSuccess, "Failed to create shader module");

            ShaderStageInfo[i].stage  = Shaders[i].Stage;
            ShaderStageInfo[i].module = Module;
            ShaderStageInfo[i].pName  = "main";
        }
    }

    std::array<vk::VertexInputBindingDescription, 1> VertexInputBindings {
        {{// binding 0
          .binding = 0,
          // pos, norm, tex coord
          .stride    = VertexBufferLayout.GetStride(),
          .inputRate = vk::VertexInputRate::eVertex}}};


    std::array<vk::VertexInputAttributeDescription, 2> VertexAttributes {
        {
         // pos, 2 floats
         {
         .binding  = VertexInputBindings[0].binding,
         .format   = vk::Format::eR32G32Sfloat,
         },

         // color, 3 floats
    {
         .binding  = VertexInputBindings[0].binding,
    .format  = vk::Format::eR32G32B32Sfloat,
    },
         //// tex coord, 2 floats
         //{
         //	.location = 2,
         //	.binding = VertexInputBindings[0].binding,
         //	.format = vk::Format::eR32G32Sfloat,
         //	.offset = sizeof(float) * (3 + 3)
         //},
        }
    };
    const auto VertexElements = VertexBufferLayout.GetElements();
    for (int i = 0; i < VertexElements.size(); i++)
    {
        VertexAttributes[i].location = i;
        VertexAttributes[i].offset   = VertexElements[i].Offset;
    }

    vk::PipelineVertexInputStateCreateInfo VertexInputStateInfo {
        .vertexBindingDescriptionCount   = static_cast<uint32_t>(VertexInputBindings.size()),
        .pVertexBindingDescriptions      = VertexInputBindings.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributes.size()),
        .pVertexAttributeDescriptions    = VertexAttributes.data()};

    // Created graphics pipeline
    m_Pipeline.Create(ShaderStageInfo, PipelineLayout, VertexInputStateInfo, Renderpass, LogicalDevice);

    // clean up Shader stage
    for (const auto& ShaderStage : ShaderStageInfo)
    {
        LogicalDevice.destroyShaderModule(ShaderStage.module);
    }
}
void Shader::Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint)
{
    CommandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_Pipeline.m_Layout,
        0,
        1,
        &m_DescriptorSet[m_CurrentDescriptorSet],
        0,
        nullptr);

    m_Pipeline.Bind(CommandBuffer, BindPoint);
   
}
void Shader::UploadUniform(
    vk::CommandBuffer CommandBuffer, CameraTransform Transform)
{
    m_CurrentDescriptorSet = (m_CurrentDescriptorSet + 1) % m_MaxDescriptorSets;

    VT_CORE_INFO(
        "Projection {}\n View {}",
        glm::to_string(Transform.ProjectionMatrix),
        glm::to_string(Transform.ViewMatrix));
    uint32_t DataSize = sizeof(Transform);
    uint32_t Offset   = 0;

    m_UBO_Buffer.LoadData(&Transform, DataSize, {}, Offset, {});

    vk::DescriptorBufferInfo BufferInfo
    {
        .buffer = m_UBO_Buffer.Get(),
        .offset = Offset,
        .range  = DataSize,
    };

    vk::WriteDescriptorSet WriteDescriptorSet
    {
        .dstSet          = m_DescriptorSet[m_CurrentDescriptorSet],
        .dstBinding      = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &BufferInfo,
    };

    m_LogicalDevice.updateDescriptorSets(1, &WriteDescriptorSet, 0, nullptr);

    // VT_CORE_TRACE("Descriptor set info updated");
}
void Shader::Destroy()
{
    m_Pipeline.Destroy();
    m_UBO_Buffer.Destroy();

    m_LogicalDevice.destroyDescriptorSetLayout(m_DescriptorLayout);
    m_LogicalDevice.destroyDescriptorPool(m_DescriptorPool);
    m_DescriptorPool   = VK_NULL_HANDLE;
    m_DescriptorLayout = VK_NULL_HANDLE;
}
Shader::~Shader() { Destroy(); }
} // namespace VT::Vulkan
