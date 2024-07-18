module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Buffer;

import VT.Buffer;

export namespace VT::Vulkan
{
class VulkanBuffer final : public VT::Buffer
{
public:
    void Create(const vk::BufferCreateInfo& Info, vk::Device Device);

    vk::CommandBuffer static BeginSingleTimeCommand(vk::CommandPool CmdPool, vk::Device Device);
    void static EndSingleTimeCommand(vk::CommandBuffer CmdBuffer,
                                     vk::CommandPool CmdPool,
                                     vk::Queue Queue,
                                     vk::Device Device);

    // Data op
    void LoadData(const void* Data, uint32_t Size, uint32_t Offset, vk::MemoryMapFlags Flags = {}) const;
    void* MapData(uint32_t Offset, uint32_t Size, vk::MemoryMapFlags Flags = {}) const;
    void UnMapData() const;
    void CopyTo(vk::Buffer Dest, vk::Queue Queue, vk::CommandPool CmdPool, vk::BufferCopy Region) const;

    void BindMem(uint32_t Offset,
                 const vk::PhysicalDeviceMemoryProperties& PDMemProperties,
                 vk::MemoryPropertyFlags MemProperties);

    // Configure op
    void BindMem(vk::DeviceMemory Mem, uint32_t Offset);
    virtual void Bind() const override;
    virtual void UnBind() const override;

    vk::BufferCreateInfo CreateInfo;
    vk::Buffer Buffer {VK_NULL_HANDLE};
    vk::DeviceMemory BufferMemory {VK_NULL_HANDLE};

public:
    VulkanBuffer()                               = default;
    VulkanBuffer(const VulkanBuffer&)            = delete;
    VulkanBuffer& operator=(VulkanBuffer& Other) = delete;
    VulkanBuffer& operator=(VulkanBuffer&& Other) noexcept;
    VulkanBuffer(VulkanBuffer&& Other) noexcept;
    void Destroy();
    ~VulkanBuffer();

private:
    vk::Device m_LogicalDevice;
};

} // namespace VT::Vulkan
