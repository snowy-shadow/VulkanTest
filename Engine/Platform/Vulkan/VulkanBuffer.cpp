module;
#include "Vulkan.h"

module VT.Platform.Vulkan.Buffer;

import VT.Log;
import VT.Platform.Vulkan.Util;

namespace VT::Vulkan
{
void VulkanBuffer::Create(const vk::BufferCreateInfo& Info, vk::Device Device)
{
    vk::Result Result;
    std::tie(Result, Buffer) = Device.createBuffer(Info);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create buffer");

    CreateInfo      = Info;
    m_LogicalDevice = Device;
}

vk::CommandBuffer VulkanBuffer::BeginSingleTimeCommand(vk::CommandPool CmdPool, vk::Device Device)
{
    vk::CommandBufferAllocateInfo AllocInfo {
        .commandPool = CmdPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1};

    const auto [Result, CmdBufferList] = Device.allocateCommandBuffers(AllocInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate command buffer");

    const auto CmdBuffer = CmdBufferList.front();

    vk::CommandBufferBeginInfo BeginInfo {.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};

    VK_CHECK(CmdBuffer.begin(BeginInfo), vk::Result::eSuccess, "Failed to begin command buffer");

    return CmdBuffer;
}

void VulkanBuffer::EndSingleTimeCommand(vk::CommandBuffer CmdBuffer,
                                        vk::CommandPool CmdPool,
                                        vk::Queue Queue,
                                        vk::Device Device)
{
    VK_CHECK(CmdBuffer.end(), vk::Result::eSuccess, "Failed to end command buffer");

    vk::SubmitInfo SubmitInfo {.commandBufferCount = 1, .pCommandBuffers = &CmdBuffer};

    VK_CHECK(Queue.submit(SubmitInfo, VK_NULL_HANDLE), vk::Result::eSuccess, "Failed to submit to queue");

    VK_CHECK(Queue.waitIdle(), vk::Result::eSuccess, "Failed to wait on queue");

    Device.freeCommandBuffers(CmdPool, CmdBuffer);
}

void VulkanBuffer::BindMem(vk::DeviceMemory Mem, uint32_t Offset)
{
    VK_CHECK(m_LogicalDevice.bindBufferMemory(Buffer, BufferMemory, Offset),
             vk::Result::eSuccess,
             "Failed to bind buffer memory");
    BufferMemory = Mem;
}

void VulkanBuffer::Bind() const {}

void VulkanBuffer::UnBind() const {}

void Vulkan::VulkanBuffer::BindMem(uint32_t Offset,
                                   const vk::PhysicalDeviceMemoryProperties& PDMemProperties,
                                   vk::MemoryPropertyFlags MemProperties)
{
    // Allocate memory
    vk::MemoryRequirements MemRequirements = m_LogicalDevice.getBufferMemoryRequirements(Buffer);
    auto [bResult, MemTypeIndex] = FindMemoryTypeIndex(PDMemProperties, MemRequirements.memoryTypeBits, MemProperties);

    vk::MemoryAllocateInfo AllocInfo {.allocationSize = MemRequirements.size, .memoryTypeIndex = MemTypeIndex};

    vk::Result Result;
    std::tie(Result, BufferMemory) = m_LogicalDevice.allocateMemory(AllocInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate buffer memory");

    VK_CHECK(m_LogicalDevice.bindBufferMemory(Buffer, BufferMemory, Offset),
             vk::Result::eSuccess,
             "Failed to bind buffer memory");
}

void VulkanBuffer::LoadData(const void* Data, uint32_t Size, uint32_t Offset, vk::MemoryMapFlags Flags) const
{
    void* BufferData = MapData(Offset, Size, Flags);
    std::memcpy(BufferData, Data, Size);
    UnMapData();
}

void* VulkanBuffer::MapData(uint32_t Offset, uint32_t Size, vk::MemoryMapFlags Flags) const
{
    auto [Result, Value] = m_LogicalDevice.mapMemory(BufferMemory, Offset, Size, Flags);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to map buffer memory");

    return Value;
}

void VulkanBuffer::UnMapData() const { m_LogicalDevice.unmapMemory(BufferMemory); }

void VulkanBuffer::CopyTo(vk::Buffer Dest, vk::Queue Queue, vk::CommandPool CmdPool, vk::BufferCopy Region) const
{
    auto CmdBuffer = BeginSingleTimeCommand(CmdPool, m_LogicalDevice);

    CmdBuffer.copyBuffer(Buffer, Dest, Region);

    EndSingleTimeCommand(CmdBuffer, CmdPool, Queue, m_LogicalDevice);
}

VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& Other) noexcept
{
    CreateInfo         = std::move(Other.CreateInfo);
    Buffer             = std::move(Other.Buffer);
    BufferMemory       = std::move(Other.BufferMemory);
    BufferLayout       = std::move(Other.BufferLayout);
    m_LogicalDevice    = std::move(Other.m_LogicalDevice);
    Other.Buffer       = VK_NULL_HANDLE;
    Other.BufferMemory = VK_NULL_HANDLE;

    return *this;
}
VulkanBuffer::VulkanBuffer(VulkanBuffer&& Other) noexcept :
    CreateInfo(std::move(Other.CreateInfo)),
    Buffer(Other.Buffer),
    BufferMemory(Other.BufferMemory),
    m_LogicalDevice(Other.m_LogicalDevice)
{
    BufferLayout       = Other.BufferLayout;
    Other.Buffer       = VK_NULL_HANDLE;
    Other.BufferMemory = VK_NULL_HANDLE;
}

void VulkanBuffer::Destroy()
{
    m_LogicalDevice.destroyBuffer(Buffer);
    m_LogicalDevice.freeMemory(BufferMemory);

    Buffer       = VK_NULL_HANDLE;
    BufferMemory = VK_NULL_HANDLE;
}
VulkanBuffer::~VulkanBuffer() { Destroy(); }
} // namespace VT::Vulkan
