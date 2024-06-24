#include "Buffer.h"
#include <utility>

namespace VT
{
	Buffer::Buffer(vk::Device D) : m_LogicalDevice(D){}

	void Buffer::bindDevice(vk::Device D)
	{
		if(D != m_LogicalDevice){ destroy(); }
		m_LogicalDevice = D;
	}

	void* Buffer::map(vk::DeviceSize Offset, vk::DeviceSize Size)
	{
		return m_LogicalDevice.mapMemory(m_Mem, Offset, Size);	
	}

	void Buffer::unmap()
	{
		m_LogicalDevice.unmapMemory(m_Mem);
	}

	std::pair<vk::Buffer&, vk::DeviceMemory&> Buffer::createBuffer(
		const vk::BufferCreateInfo& Info,
		vk::PhysicalDeviceMemoryProperties PD_Mem,
		vk::MemoryPropertyFlags Properties)
	{
		m_Buffer = m_LogicalDevice.createBuffer(Info);

		auto MemReq = m_LogicalDevice.getBufferMemoryRequirements(m_Buffer);

		vk::MemoryAllocateInfo MemAllocInfo
		{
			.allocationSize = MemReq.size,
			.memoryTypeIndex = findMemoryTypeIndex(PD_Mem, MemReq.memoryTypeBits, Properties)
		};

		m_Mem = m_LogicalDevice.allocateMemory(MemAllocInfo);

		m_LogicalDevice.bindBufferMemory(m_Buffer, m_Mem, 0);

		return { m_Buffer, m_Mem };
	}

	vk::Buffer Buffer::getBuffer() { return m_Buffer; }
	vk::DeviceMemory Buffer::getMem() { return m_Mem; }
	vk::BufferView Buffer::getView() { return m_View; }
	void* Buffer::getMappedPtr() { return m_Data; }

	Buffer::Buffer(Buffer&& Other) noexcept :
		m_Buffer(Other.m_Buffer), m_View(Other.m_View), m_Data(Other.m_Data)
	{
		Other.m_View = VK_NULL_HANDLE;
		Other.m_Buffer = VK_NULL_HANDLE;
		Other.m_Data = nullptr;
	}

	Buffer& Buffer::operator = (Buffer&& Other) noexcept
	{
		m_Buffer = std::exchange(Other.m_Buffer, VK_NULL_HANDLE);
		m_View = std::exchange(Other.m_View, VK_NULL_HANDLE);
		m_Data = std::exchange(Other.m_Data, nullptr);
		return *this;
	}

	uint32_t Buffer::findMemoryTypeIndex(vk::PhysicalDeviceMemoryProperties MemProperties, uint32_t TypeFilter, vk::MemoryPropertyFlags Usage) const
	{
		for(uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
		{
			if((TypeFilter & 1 << i) && (Usage & MemProperties.memoryTypes[i].propertyFlags) == Usage)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memeory type index\n");
	}
	void Buffer::destroy() noexcept
	{

		if(m_View != VK_NULL_HANDLE) { m_LogicalDevice.destroyBufferView(m_View); }
		if(m_Buffer != VK_NULL_HANDLE) { m_LogicalDevice.destroyBuffer(m_Buffer); }
	}

	Buffer::~Buffer()
	{
		destroy();
	}

}
