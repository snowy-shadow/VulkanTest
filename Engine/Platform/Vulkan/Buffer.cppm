module;
#include "Vulkan.h"
export module VT.Platform.Vulkan.Buffer;

import VT.Buffer;

export namespace VT::Vulkan
{
class Buffer : public VT::Buffer
	{
    public:
        void Create(const vk::BufferCreateInfo& Info, vk::Device Device);

        // Data op
        void LoadData(void* Data, uint32_t Size, BufferLayout Layout, uint32_t Offset, vk::MemoryMapFlags Flags = {});
        void* MapData(uint32_t Offset, uint32_t Size, vk::MemoryMapFlags Flags = {}) const;
        void UnMapData() const;
        void CopyTo(vk::Buffer Dest, vk::Queue Queue, vk::CommandPool CmdPool, vk::BufferCopy Region) const;

        void BindMem(
            uint32_t Offset,
            const vk::PhysicalDeviceMemoryProperties& PDMemProperties,
            vk::MemoryPropertyFlags MemProperties);

        // Configure op
        void BindMem(vk::DeviceMemory Mem, uint32_t Offset);
        virtual void Bind() const override;
        virtual void UnBind() const override;
        virtual void SetLayout(BufferLayout) override;


        virtual constexpr BufferLayout GetLayout() const override;
        vk::Buffer Get() const;
        vk::BufferCreateInfo GetInfo() const;

    public:
        Buffer()                         = default;
        Buffer(const Buffer&)            = delete;
        Buffer& operator=(Buffer& Other) = delete;
        Buffer& operator=(Buffer&& Other) noexcept;
        Buffer(Buffer&& Other) noexcept;
        void Destroy();
        ~Buffer();
    private:
        vk::BufferCreateInfo m_Info;
        VT::BufferLayout m_Layout;
        vk::Buffer m_Buffer;
        vk::DeviceMemory m_BufferMemory;

        vk::Device m_LogicalDevice;

	};

    } // namespace VT::Vulkan
