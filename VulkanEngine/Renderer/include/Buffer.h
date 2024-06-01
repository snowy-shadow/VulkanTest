#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace VT
{
	class Buffer
	{
	public:
		explicit Buffer(vk::Device);

		std::pair<vk::Buffer&, vk::DeviceMemory&> createBuffer(
			const vk::BufferCreateInfo& Info,
			vk::PhysicalDeviceMemoryProperties,
			vk::MemoryPropertyFlags Properties);
	
		void* map(vk::DeviceSize Offset, vk::DeviceSize Size);
		void unmap();

		void bindDevice(vk::Device);

		vk::Buffer getBuffer();
		vk::DeviceMemory getMem();
		vk::BufferView getView();
		void* getMappedPtr();

		Buffer(Buffer&&) noexcept;
		Buffer& operator = (Buffer&&) noexcept;
		~Buffer();
	
		Buffer(Buffer&) = delete;
		Buffer& operator = (const Buffer&) = delete;
	private:
		void destroy() noexcept;
		uint32_t findMemoryTypeIndex(
			vk::PhysicalDeviceMemoryProperties MemProperties,
	   	   uint32_t TypeFilter,
		   vk::MemoryPropertyFlags Usage) const;
	
		vk::Buffer m_Buffer;
		vk::DeviceMemory m_Mem;
		vk::BufferView m_View;
		vk::Device m_LogicalDevice;
		void* m_Data{nullptr};
	};
}
