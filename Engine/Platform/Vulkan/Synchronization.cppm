module;
#include "Vulkan.h"

export module VT.Platform.Vulkan.Synchronization;
import VT.Log;

export namespace VT::Vulkan
{
class Fence
{
public:
	Fence() = default;

	vk::Fence Create(vk::Device Device, vk::FenceCreateFlags Flags = vk::FenceCreateFlags{})
	{
		Destroy();
		m_Device = Device;

		vk::FenceCreateInfo FenceInfo;
		FenceInfo.flags = Flags;

		vk::Result Result;
		std::tie(Result, m_Fence) = Device.createFence(FenceInfo);
		VK_CHECK(Result, vk::Result::eSuccess, "Failed to create fence");

		return m_Fence;
	}

	bool Wait(uint64_t Timeout = std::numeric_limits<uint64_t>::max()) const
	{
		const auto Result = m_Device.waitForFences(m_Fence, VK_TRUE, Timeout);

		VK_CHECK(Result, vk::Result::eSuccess, "Failed to wait for fence");

		return Result == vk::Result::eSuccess;
	}

	bool Reset() const
	{
		const auto Result = m_Device.resetFences(m_Fence);

		VK_CHECK(Result, vk::Result::eSuccess, "Failed to reset fence");

		return Result == vk::Result::eSuccess;
	}

    vk::Fence Get() const { return m_Fence; }

	void Destroy()
	{
		if (m_Fence != VK_NULL_HANDLE)
		{
			m_Device.destroyFence(m_Fence);
		}
	}

	~Fence() { Destroy(); }

private:
	vk::Fence m_Fence;
	vk::Device m_Device;
};
} // namespace VT::Vulkan
