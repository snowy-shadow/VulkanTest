module;
#include "Vulkan.h"

module VT.Platform.Vulkan.Buffer;

import VT.Log;
import VT.Platform.Vulkan.Util;

namespace VT::Vulkan
{
void Buffer::Create(
    const vk::BufferCreateInfo& Info,
    vk::PhysicalDeviceMemoryProperties PDMemProperties,
    vk::MemoryPropertyFlags MemProperties,
    vk::Device Device)
{
    vk::Result Result;
    std::tie(Result, m_Buffer) = Device.createBuffer(Info);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create buffer");
    vk::MemoryRequirements MemRequirements = Device.getBufferMemoryRequirements(m_Buffer);

    // Allocate memory
    auto [bResult, MemTypeIndex] = FindMemoryTypeIndex(PDMemProperties, MemRequirements.memoryTypeBits, MemProperties);

    vk::MemoryAllocateInfo AllocInfo {.allocationSize = MemRequirements.size, .memoryTypeIndex = MemTypeIndex};

    std::tie(Result, m_BufferMemory) = Device.allocateMemory(AllocInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate buffer memory");


    m_Info          = Info;
    m_LogicalDevice = Device;
}
void* Buffer::MapData(uint32_t Offset, uint32_t Size, vk::MemoryMapFlags Flags) const
{
    auto [Result, Value] = m_LogicalDevice.mapMemory(m_BufferMemory, Offset, Size, Flags);
    VK_CHECK(Result,
        vk::Result::eSuccess,
        "Failed to map buffer memory");

    return Value;
}

void Buffer::UnMapData() const { m_LogicalDevice.unmapMemory(m_BufferMemory); }

void Buffer::CopyTo(vk::Queue Queue, vk::CommandPool CmdPool, vk::BufferCopy Region, vk::Buffer Dest) const
{
    VK_CHECK(Queue.waitIdle(), vk::Result::eSuccess, "Copy buffer failed to wait on Queue");

    vk::CommandBufferAllocateInfo CommandBufferInfo {
        .commandPool        = CmdPool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1};

    auto [Result, CopyCmdBuffer] = m_LogicalDevice.allocateCommandBuffers(CommandBufferInfo);

    auto& Buffer = CopyCmdBuffer.front();

    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create buffer copy command buffer");

    Buffer.copyBuffer(m_Buffer, Dest, Region);

    VK_CHECK(Buffer.end(), vk::Result::eSuccess, "Copy buffer command buffer failed to end");
}

void Buffer::Bind(uint32_t Offset)
{
    VK_CHECK(
        m_LogicalDevice.bindBufferMemory(m_Buffer, m_BufferMemory, Offset),
        vk::Result::eSuccess,
        "Failed to bind buffer memory");
}

vk::Buffer Buffer::Get() const { return m_Buffer; }

vk::BufferCreateInfo Buffer::GetInfo() const { return m_Info; }

void Buffer::Destroy()
{
    if (m_Buffer != VK_NULL_HANDLE)
    {
        m_LogicalDevice.destroyBuffer(m_Buffer);
        m_LogicalDevice.freeMemory(m_BufferMemory);

        m_Buffer       = VK_NULL_HANDLE;
        m_BufferMemory = VK_NULL_HANDLE;
    }
}
Buffer::~Buffer() { Destroy(); }
} // namespace VT::Vulkan
