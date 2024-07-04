module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Buffer;

export namespace VT::Vulkan
{
	class Buffer
	{
    public:
        void Create(
            const vk::BufferCreateInfo& Info,
            vk::PhysicalDeviceMemoryProperties PDMemProperties,
            vk::MemoryPropertyFlags MemProperties,
            vk::Device Device);

        void* MapData(uint32_t Offset, uint32_t Size, vk::MemoryMapFlags Flags) const;
        void UnMapData() const;

        void CopyTo(vk::Queue Queue, vk::CommandPool CmdPool, vk::BufferCopy Region, vk::Buffer Dest) const;

        void Bind(uint32_t Offset);

        vk::Buffer Get() const;
        vk::BufferCreateInfo GetInfo() const;

        void Destroy();
        ~Buffer();
    private:
        vk::BufferCreateInfo m_Info;
        vk::Buffer m_Buffer;
        vk::DeviceMemory m_BufferMemory;

        vk::Device m_LogicalDevice;

	};

    } // namespace VT::Vulkan
