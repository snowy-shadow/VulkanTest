#include "PhysicalDevice.h"

namespace  VT
{
	vk::Device PhysicalDevice::createLogicalDevice(const std::vector<const char*>& DeviceExtensionName)
	{
		vk::DeviceCreateInfo DeviceInfo
		{
			.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueues.size()),
			.pQueueCreateInfos = m_DeviceQueues.data(),
			.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensionName.size()),
			.ppEnabledExtensionNames = DeviceExtensionName.data()
		};

		return m_PhysicalDevice.createDevice(DeviceInfo);
	}

	void PhysicalDevice::addQueue(const vk::QueueFlagBits& RequiredQueue, const float& QueuePriority, const uint32_t& QueueCount)
	{
		auto Queue{ findQueue(RequiredQueue, QueueCount) };

		// check if graphics queue
		if (RequiredQueue & vk::QueueFlagBits::eGraphics) { m_GraphicsQueue = Queue.at(0); }

		m_DeviceQueues.emplace_back(vk::DeviceQueueCreateInfo
		{
			.queueFamilyIndex = Queue.at(0),
			.queueCount = Queue.at(1),
			.pQueuePriorities = &QueuePriority
		});
	}

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


	bool PhysicalDevice::findPresentQueue()
	{
		assert(m_Surface);

		// check if get queue already found a potential match
		if (!m_GraphicsQueue.has_value())
		{
			addQueue(vk::QueueFlagBits::eGraphics, 0.0f, 1);
		}

		// prioritize graphics and present queue to be same index
		if (m_PhysicalDevice.getSurfaceSupportKHR(m_GraphicsQueue.value(), m_Surface))
		{
			m_PresentQueue = { m_GraphicsQueue.value(), true };
			return true;
		}

		size_t Size{ m_PhysicalDevice.getQueueFamilyProperties().size() };

		for (auto Index = 0; Index < Size; Index++)
		{
			if (m_PhysicalDevice.getSurfaceSupportKHR(Index, m_Surface))
			{
				m_PresentQueue = std::tuple{ Index, false };
				return true;
			}
		}

		return false;
	}

	void PhysicalDevice::bindSurface(const vk::SurfaceKHR& Surface)
	{
		m_Surface = Surface;
	}


	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

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

	std::array<uint32_t, 2> PhysicalDevice::findQueue(const vk::QueueFlagBits& RequiredQueue, const uint32_t& QueueCount) const
	{
		auto QueueFamilies{ m_PhysicalDevice.getQueueFamilyProperties() };

		auto Iterator{ std::find_if(QueueFamilies.cbegin(), QueueFamilies.cend(),
		[RequiredQueue, QueueCount](const vk::QueueFamilyProperties& QFP)
		{
			return (QFP.queueFlags & RequiredQueue) && (QFP.queueCount >= QueueCount);

		}) };

		if (Iterator == QueueFamilies.cend()){throw std::runtime_error("No compatible physical device queue family");}

		auto Index{ static_cast<uint32_t>(std::distance(QueueFamilies.cbegin(), Iterator)) };

		return { Index, QueueFamilies.at(Index).queueCount };
	}
}
