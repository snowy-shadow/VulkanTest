module;
#include "Vulkan.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
module VT.Platform.Vulkan.Shader;
import VT.Log;

namespace VT::Vulkan
{
void Shader::Create(std::span<const HLSL::ShaderFileInfo> Shaders,
                    uint32_t MaxDescriptorSets,
                    BufferLayout UniformBufferLayout,
                    BufferLayout VertexBufferLayout,
                    vk::RenderPass Renderpass,
                    vk::PhysicalDevice PhysicalDevice,
                    vk::Device LogicalDevice)
{
    m_LogicalDevice = LogicalDevice;
    /* ===================================================================
     *                    Descriptor set
     * ===================================================================
     */
    {
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
            .descriptorCount = MaxDescriptorSets,
        };

        vk::DescriptorPoolCreateInfo PoolInfo {
            .maxSets       = MaxDescriptorSets,
            .poolSizeCount = 1,
            .pPoolSizes    = &PoolSize,
        };

        std::tie(Result, m_DescriptorPool) = LogicalDevice.createDescriptorPool(PoolInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor pool");

        m_UBO_Buffer.Create({.size  = UniformBufferLayout.GetStride(),
                             .usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst},
                            LogicalDevice);

        m_UBO_Buffer.BindMem(0,
                             PhysicalDevice.getMemoryProperties(),
                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent |
                                 vk::MemoryPropertyFlagBits::eDeviceLocal);

        // Allocate descriptor set
        vk::DescriptorSetLayout pDescriptorSetLayout[] {m_DescriptorLayout, m_DescriptorLayout, m_DescriptorLayout};
        vk::DescriptorSetAllocateInfo DescriptorSetAllocInfo {
            .descriptorPool     = m_DescriptorPool,
            .descriptorSetCount = MaxDescriptorSets,
            .pSetLayouts        = pDescriptorSetLayout,
        };

        std::tie(Result, m_DescriptorSet) = LogicalDevice.allocateDescriptorSets(DescriptorSetAllocInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate descriptor set");
    }
    VT_CORE_TRACE("Descriptor set created");

    /* ===================================================================
     *                    Local Descriptor set
     * ===================================================================
     */
    {
        constexpr uint32_t LocalSamplerCount = 1;

        std::array<vk::DescriptorType, MaxObjectDescriptor> DescriptorType {vk::DescriptorType::eUniformBuffer,
                                                                            vk::DescriptorType::eCombinedImageSampler};

        std::array<vk::DescriptorSetLayoutBinding, ObjectShaderDescriptorCount> DescriptorLayoutBindings;

        for (uint32_t i = 0; i < ObjectShaderDescriptorCount; i++)
        {
            DescriptorLayoutBindings[i].binding         = i;
            DescriptorLayoutBindings[i].descriptorCount = 1;
            DescriptorLayoutBindings[i].descriptorType  = DescriptorType[i];
            DescriptorLayoutBindings[i].stageFlags      = vk::ShaderStageFlagBits::eFragment;
        }

        vk::DescriptorSetLayoutCreateInfo DescriptorLayoutInfo {
            .bindingCount = DescriptorLayoutBindings.size(),
            .pBindings    = DescriptorLayoutBindings.data(),
        };

        vk::Result Result;
        std::tie(Result, m_ObjectDescriptorLayout) = LogicalDevice.createDescriptorSetLayout(DescriptorLayoutInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor set layout");

        std::array<vk::DescriptorPoolSize, 2> PoolSize;

        PoolSize[0].type            = vk::DescriptorType::eUniformBuffer;
        PoolSize[0].descriptorCount = MaxObjectDescriptor;

        PoolSize[1].type            = vk::DescriptorType::eCombinedImageSampler;
        PoolSize[1].descriptorCount = LocalSamplerCount * MaxObjectDescriptor;

        vk::DescriptorPoolCreateInfo PoolInfo {
            .maxSets       = MaxObjectDescriptor,
            .poolSizeCount = PoolSize.size(),
            .pPoolSizes    = PoolSize.data(),
        };

        std::tie(Result, m_ObjectDescriptorPool) = LogicalDevice.createDescriptorPool(PoolInfo);
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to create descriptor pool");

        m_ObjectUniformBuffer.Create(
            {.size  = sizeof(UniformObjectData),
             .usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst},
            LogicalDevice);
        m_ObjectUniformBuffer.BindMem(0,
                                      PhysicalDevice.getMemoryProperties(),
                                      vk::MemoryPropertyFlagBits::eHostVisible |
                                          vk::MemoryPropertyFlagBits::eHostCoherent |
                                          vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    // Push constant
    vk::PushConstantRange PushConstantInfo {
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset     = 0,
        .size       = sizeof(GeometryRenderData::Model),
    };

    std::array DescriptorArray {m_DescriptorLayout, m_ObjectDescriptorLayout};
    vk::PipelineLayoutCreateInfo PipelineLayout {
        .setLayoutCount         = DescriptorArray.size(),
        .pSetLayouts            = DescriptorArray.data(),
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &PushConstantInfo,
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
          .binding   = 0,
          // pos, norm, tex coord
          .stride    = VertexBufferLayout.GetStride(),
          .inputRate = vk::VertexInputRate::eVertex}}};

    std::array<vk::VertexInputAttributeDescription, 2> VertexAttributes {
        {
         // pos, 2 floats
         {
         .binding = VertexInputBindings[0].binding,
         .format  = vk::Format::eR32G32Sfloat,
         },       // color, 3 floats
       {
       .binding = VertexInputBindings[0].binding,
       .format  = vk::Format::eR32G32B32Sfloat,
       }, //// tex coord, 2 floats
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
    if (m_bDescriptorRebind)
    {
        CommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, m_Pipeline.m_Layout, 0, m_DescriptorSet[m_CurrentDescriptorSet], nullptr);
        m_bDescriptorRebind = false;
    }

    m_Pipeline.Bind(CommandBuffer, BindPoint);
}
void Shader::UploadCameraView(const UniformCameraData& Data)
{
    m_CurrentDescriptorSet = (m_CurrentDescriptorSet + 1) % m_DescriptorSet.size();

    VT_CORE_INFO("Projection {}\n View {}", glm::to_string(Data.ProjectionMatrix), glm::to_string(Data.ViewMatrix));

    uint32_t DataSize = sizeof(Data);
    uint32_t Offset   = 0;

    m_UBO_Buffer.LoadData(&Data, DataSize, Offset);

    vk::DescriptorBufferInfo BufferInfo {
        .buffer = m_UBO_Buffer.Buffer,
        .offset = Offset,
        .range  = DataSize,
    };

    vk::WriteDescriptorSet WriteDescriptorSet {
        .dstSet          = m_DescriptorSet[m_CurrentDescriptorSet],
        .dstBinding      = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &BufferInfo,
    };

    m_LogicalDevice.updateDescriptorSets(1, &WriteDescriptorSet, 0, nullptr);
    m_bDescriptorRebind = true;
}
void Shader::UploadGeometry(const GeometryRenderData& Data,
                                vk::CommandBuffer CmdBuffer,
                                const Timestep& Timestep)
{
    CmdBuffer.pushConstants(m_Pipeline.m_Layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Data.Model), &Data.Model);

    ObjectState& Obj = m_ObjectState[Data.ID];

    uint32_t Size   = sizeof(UniformObjectData);
    uint32_t Offset = sizeof(UniformObjectData) * Data.ID;
    UniformObjectData UniformObject;

    static float Accumulator = 0.f;
    Accumulator += Timestep.MilliSecond();
    float S                    = std::sin(Accumulator + 1.f) / 2.f;
    UniformObject.DiffuseColor = glm::vec4(S, S, S, 1.f);

    m_ObjectUniformBuffer.LoadData(&UniformObject, Size, Offset);

    // If descriptor have not been written
    if (Obj.CurrentDescriptorIndex == InvalidID)
    {
        std::vector<vk::WriteDescriptorSet> WriteDescriptorSet;
        vk::DescriptorSet ObjectDS = Obj.DescriptorSet[Obj.CurrentDescriptorIndex];
        vk::DescriptorBufferInfo BufferInfo {.buffer = m_ObjectUniformBuffer.Buffer, .offset = Offset, .range = Size};

        vk::WriteDescriptorSet WriteDescriptor {
            .dstSet          = ObjectDS,
            .dstBinding      = 0,
            .descriptorCount = 1,
            .descriptorType  = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo     = &BufferInfo,
        };

        WriteDescriptorSet.push_back(WriteDescriptor);
        m_LogicalDevice.updateDescriptorSets(WriteDescriptorSet, nullptr);

        CmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Pipeline.m_Layout, 0, ObjectDS, nullptr);
        Obj.CurrentDescriptorIndex = 0;
    }

    // constexpr uint32_t SamplerCount = 1;

    // vk::DescriptorImageInfo ImageInfo;

    // for (uint32_t SamplerIndex = 0; SamplerIndex < SamplerCount; SamplerIndex++)
    //{
    //     Texture& Texture = Data.pTextures[SamplerIndex];
    //     uint32_t& DescriptorGeneration = Obj.State[DescriptorIndex].Generation[ImageIndex];

    //
    //}
}
uint32_t Shader::CreateObject()
{
    uint32_t ID = m_CurrentObjectUniformBufferIndex++;

    auto& ObjectState                  = m_ObjectState[ID];
    ObjectState.CurrentDescriptorIndex = InvalidID;

    std::array DS_Layout {m_ObjectDescriptorLayout, m_ObjectDescriptorLayout, m_ObjectDescriptorLayout};

    vk::DescriptorSetAllocateInfo AllocateInfo {
        .descriptorPool     = m_ObjectDescriptorPool,
        .descriptorSetCount = DS_Layout.size(),
        .pSetLayouts        = DS_Layout.data(),
    };

    vk::Result Result;
    std::tie(Result, ObjectState.DescriptorSet) = m_LogicalDevice.allocateDescriptorSets(AllocateInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate object descriptor set : ID {}", ID);

    ObjectState.State->MaxDescriptor = ObjectState.DescriptorSet.size();
    return ID;
}
void Shader::ReleaseObject(uint32_t ID)
{
    VT_CORE_ASSERT(ID <= m_CurrentObjectUniformBufferIndex, "Invalid ID");

    auto& Object = m_ObjectState[ID];
    m_LogicalDevice.freeDescriptorSets(m_ObjectDescriptorPool, Object.DescriptorSet);
    Object.CurrentDescriptorIndex = InvalidID;
}
void Shader::Destroy()
{
    m_Pipeline.Destroy();
    m_UBO_Buffer.Destroy();

    m_LogicalDevice.destroyDescriptorSetLayout(m_DescriptorLayout);
    m_LogicalDevice.destroyDescriptorPool(m_DescriptorPool);
    m_DescriptorPool   = VK_NULL_HANDLE;
    m_DescriptorLayout = VK_NULL_HANDLE;

    m_LogicalDevice.destroyDescriptorSetLayout(m_ObjectDescriptorLayout);
    m_LogicalDevice.destroyDescriptorPool(m_ObjectDescriptorPool);
    m_ObjectDescriptorLayout = VK_NULL_HANDLE;
    m_ObjectDescriptorPool   = VK_NULL_HANDLE;
}
Shader::~Shader() { Destroy(); }
} // namespace VT::Vulkan
