module;
#include "Vulkan.h"
#include <glm/glm.hpp>
#include <list>

export module VT.Platform.Vulkan.Shader;

import VT.Platform.Vulkan.Buffer;
import VT.Platform.Vulkan.Pipeline;
import VT.Platform.Vulkan.DescriptorSet;

import VT.RendererType;
import VT.ShaderCompiler;
import VT.Buffer;
import VT.Camera;
import VT.Timestep;

constexpr uint32_t MaxObjectDescriptor = 1024;
constexpr uint32_t ObjectShaderDescriptorCount = 2;

namespace VT::Vulkan
{
struct DescriptorState
{
    uint32_t MaxDescriptor     = 0;
    
};
struct ObjectState
{
    std::vector<vk::DescriptorSet> DescriptorSet;
    ID CurrentDescriptorIndex = InvalidID;
    DescriptorState State[ObjectShaderDescriptorCount];
};
} // namespace VT::Vulkan

export namespace VT::Vulkan
{
class Shader
{
public:
    void Create(std::span<const HLSL::ShaderFileInfo> Shaders,
                uint32_t MaxDescriptorSets,
                BufferLayout UniformBufferLayout,
                BufferLayout VertexBufferLayout,
                vk::RenderPass Renderpass,
                vk::PhysicalDevice PhysicalDevice,
                vk::Device LogicalDevice);

    void Bind(vk::CommandBuffer CommandBuffer, vk::PipelineBindPoint BindPoint);

    void UploadCameraView(const UniformCameraData& Data);
    void UploadGeometry(const GeometryRenderData& Data,
                            vk::CommandBuffer CmdBuffer,
                            const Timestep& Timestep);

    uint32_t CreateObject();
    void ReleaseObject(uint32_t ID);

    void Destroy();

public:
    Shader()                                   = default;
    Shader& operator=(Shader& Other)           = delete;
    Shader& operator=(Shader&& Other) noexcept = delete;
    Shader(Shader&& Other) noexcept            = delete;
    Shader(Shader& Other)                      = delete;
    ~Shader();

private:
    Pipeline m_Pipeline;

    // Uniform
    VulkanBuffer m_UBO_Buffer;
    std::vector<vk::DescriptorSet> m_DescriptorSet;
    vk::DescriptorPool m_DescriptorPool        = VK_NULL_HANDLE;
    vk::DescriptorSetLayout m_DescriptorLayout = VK_NULL_HANDLE;
    bool m_bDescriptorRebind                   = true;

    uint32_t m_CurrentDescriptorSet = 0;

    // Material
    VulkanBuffer m_ObjectUniformBuffer;
    vk::DescriptorSetLayout m_ObjectDescriptorLayout;
    vk::DescriptorPool m_ObjectDescriptorPool;
    uint32_t m_CurrentObjectUniformBufferIndex;

    ObjectState m_ObjectState[MaxObjectDescriptor];

    vk::Device m_LogicalDevice;
};

} // namespace VT::Vulkan
