module;
#include "Vulkan.h"

module VT.Platform.Vulkan.Buffer;

import VT.Log;
import VT.Platform.Vulkan.Util;

namespace VT::Vulkan
{
void Buffer::Create(const vk::BufferCreateInfo& Info, vk::Device Device)
{
    vk::Result Result;
    std::tie(Result, m_Buffer) = Device.createBuffer(Info);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create buffer");

    m_Info          = Info;
    m_LogicalDevice = Device;
}

void Buffer::BindMem(vk::DeviceMemory Mem, uint32_t Offset)
{
    VK_CHECK(
        m_LogicalDevice.bindBufferMemory(m_Buffer, m_BufferMemory, Offset),
        vk::Result::eSuccess,
        "Failed to bind buffer memory");
    m_BufferMemory = Mem;
}

void Buffer::Bind() const {}

void Buffer::UnBind() const {}

void Buffer::SetLayout(BufferLayout Layout) { m_Layout = std::move(Layout); }

constexpr BufferLayout Buffer::GetLayout() const { return m_Layout; }

void Vulkan::Buffer::BindMem(
    uint32_t Offset,
    const vk::PhysicalDeviceMemoryProperties& PDMemProperties,
    vk::MemoryPropertyFlags MemProperties)
{
    // Allocate memory
    vk::MemoryRequirements MemRequirements = m_LogicalDevice.getBufferMemoryRequirements(m_Buffer);
    auto [bResult, MemTypeIndex] = FindMemoryTypeIndex(PDMemProperties, MemRequirements.memoryTypeBits, MemProperties);

    vk::MemoryAllocateInfo AllocInfo {.allocationSize = MemRequirements.size, .memoryTypeIndex = MemTypeIndex};

    vk::Result Result;
    std::tie(Result, m_BufferMemory) = m_LogicalDevice.allocateMemory(AllocInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to allocate buffer memory");

    VK_CHECK(
        m_LogicalDevice.bindBufferMemory(m_Buffer, m_BufferMemory, Offset),
        vk::Result::eSuccess,
        "Failed to bind buffer memory");
}


void Buffer::LoadData(void* Data, uint32_t Size, BufferLayout Layout, uint32_t Offset, vk::MemoryMapFlags Flags)
{
    void* BufferData = MapData(Offset, Size, Flags);
    std::memcpy(BufferData, Data, Size);
    UnMapData();

    m_Layout = std::move(Layout);
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

void Buffer::CopyTo(vk::Buffer Dest, vk::Queue Queue, vk::CommandPool CmdPool, vk::BufferCopy Region) const
{
    vk::CommandBufferAllocateInfo CommandBufferInfo {
        .commandPool        = CmdPool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1};

    VK_CHECK(Queue.waitIdle(), vk::Result::eSuccess, "Begin Copy buffer failed to wait on Queue");

    auto [Result, CopyCmdBuffer] = m_LogicalDevice.allocateCommandBuffers(CommandBufferInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create buffer copy command buffer");

    // Copy data
    auto& Buffer = CopyCmdBuffer.front();

    Result = Buffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    VK_CHECK(Result, vk::Result::eSuccess, "Copy buffer command buffer failed to begin");

    Buffer.copyBuffer(m_Buffer, Dest, Region);

    VK_CHECK(Buffer.end(), vk::Result::eSuccess, "Copy buffer command buffer failed to end");


    // Queue submit
    {
        vk::SubmitInfo RenderSubmit {.commandBufferCount = 1, .pCommandBuffers = &Buffer};

        VK_CHECK(Queue.submit(RenderSubmit, VK_NULL_HANDLE), vk::Result::eSuccess, "Failed to submit to graphic queue");
        // end Queue submit
    }

    VK_CHECK(Queue.waitIdle(), vk::Result::eSuccess, "End Copy buffer failed to wait on Queue");

    // Free the command buffer.
    m_LogicalDevice.freeCommandBuffers(CmdPool, CopyCmdBuffer);
}


vk::Buffer Buffer::Get() const { return m_Buffer; }

vk::BufferCreateInfo Buffer::GetInfo() const { return m_Info; }

Buffer& Buffer::operator=(Buffer&& Other) noexcept
{
    m_Info               = Other.m_Info;
    m_Buffer             = Other.m_Buffer;
    m_BufferMemory       = Other.m_BufferMemory;
    m_LogicalDevice      = Other.m_LogicalDevice;
    Other.m_Buffer       = VK_NULL_HANDLE;
    Other.m_BufferMemory = VK_NULL_HANDLE;

    return *this;
}
Buffer::Buffer(Buffer&& Other) noexcept :
    m_Info(Other.m_Info),
    m_Buffer(Other.m_Buffer),
    m_BufferMemory(Other.m_BufferMemory),
    m_LogicalDevice(Other.m_LogicalDevice)
{
    Other.m_Buffer       = VK_NULL_HANDLE;
    Other.m_BufferMemory = VK_NULL_HANDLE;
}

void Buffer::Destroy()
{
    m_LogicalDevice.destroyBuffer(m_Buffer);
    m_LogicalDevice.freeMemory(m_BufferMemory);

    m_Buffer       = VK_NULL_HANDLE;
    m_BufferMemory = VK_NULL_HANDLE;
}
Buffer::~Buffer() { Destroy(); }
} // namespace VT::Vulkan
