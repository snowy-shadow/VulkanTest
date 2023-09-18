#include "PhysicalDevice.h"

namespace  VT
{
	vk::Device PhysicalDevice::createLogicalDevice(
		const std::vector<vk::PhysicalDevice>& DeviceList, 
		const std::vector<const char*>& RequiredExtensions,
		const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues,
		const vk::SurfaceKHR& Surface)
	{
		if(!findPhysicalDevice(std::forward<decltype(DeviceList)>(DeviceList), std::forward<decltype(RequiredExtensions)>(RequiredExtensions)))
		{
			throw std::runtime_error("Cannot find compatible physical device");
		}

		for(const auto& Q : RequiredQueues)
		{
			addQueue(std::get<vk::QueueFlagBits>(Q), std::get<float>(Q));
		}

		vk::DeviceCreateInfo DeviceInfo
		{
			.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueues.size()),
			.pQueueCreateInfos = m_DeviceQueues.data()
		};

		return m_PhysicalDevice.createDevice(DeviceInfo);
	}

	void PhysicalDevice::addQueue(const vk::QueueFlagBits& RequiredQueue, const float& QueuePriority, const uint32_t& QueueCount)
	{
		auto Queue{ findQueue(RequiredQueue, QueueCount) };

		m_DeviceQueues.emplace_back(vk::DeviceQueueCreateInfo
		{
			.queueFamilyIndex = Queue.at(0),
			.queueCount = Queue.at(1),
			.pQueuePriorities = &QueuePriority
		});
	}

	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */
	bool PhysicalDevice::findPhysicalDevice(const std::vector<vk::PhysicalDevice>& DeviceList, const std::vector<const char*>& RequiredExtensions)
	{
		for(const auto& Device : DeviceList)
		{
			if(extensionSupport(Device, RequiredExtensions))
			{
				m_PhysicalDevice = Device;
				return true;
			}
		}
		return false;
	}

	bool PhysicalDevice::extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions) const
	{
		// construct set
		std::unordered_set<std::string> DeviceSupportedExtentions;
		for(const auto& Extension : PhysicalDevice.enumerateDeviceExtensionProperties())
		{
			DeviceSupportedExtentions.insert(Extension.extensionName);
		}

		// check if set contains all required extentions
		for (const auto& Extension : RequiredExtensions)
		{
			if(!DeviceSupportedExtentions.contains(Extension))
			{
				return false;
			}
		}

		return true;
	}

	uint32_t PhysicalDevice::findPresentQueueIndex(const vk::SurfaceKHR& Surface) const
	{
		for(const auto& Index : m_PhysicalDevice.getQueueFamilyProperties().size())
		{
			if(m_PhysicalDevice.getSurfaceSupportKHR(Index, Surface))
			{
				return Index;
			}
		}

		throw std::runtime_error("Cannot find Present queue");
	}

	std::array<uint32_t, 2> PhysicalDevice::findQueue(const vk::QueueFlagBits& RequiredQueue, const uint32_t& QueueCount)
	{
		auto QueueFamilies{ m_PhysicalDevice.getQueueFamilyProperties() };

		auto Iterator{ std::find_if(QueueFamilies.cbegin(), QueueFamilies.cend(),
		[RequiredQueue, QueueCount](const vk::QueueFamilyProperties& QFP)
		{
			return (QFP.queueFlags & RequiredQueue) && (QFP.queueCount >= QueueCount);

		}) };

		if (Iterator == QueueFamilies.cend()){throw std::runtime_error("No compatible physical device queue family");}

		auto Index{ static_cast<uint32_t>(std::distance(QueueFamilies.cbegin(), Iterator)) };

		// check if graphics queue can present
		if (RequiredQueue == vk::QueueFlagBits::eGraphics)
		{
			m_PresentQueueInfo = m_PhysicalDevice.getSurfaceSupportKHR(Index, m_Surface) ? std::tuple{ Index, true } : std::tuple{ findPresentQueueIndex(m_Surface), false };
		}

		return { Index, QueueFamilies.at(Index).queueCount };
	}
}
