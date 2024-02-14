#include "PhysicalDevice.h"

#include <unordered_set>
#include <array>

namespace  VT
{
	vk::Device PhysicalDevice::createLogicalDevice(const std::vector<const char*>& DeviceExtensionName)
	{
		// graphics queue
		m_DeviceQueues.push_back(m_GraphicsQueue.value());

		vk::DeviceCreateInfo DeviceInfo
		{
			.queueCreateInfoCount = static_cast<uint32_t>(m_DeviceQueues.size()),
			.pQueueCreateInfos = m_DeviceQueues.data(),
			.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensionName.size()),
			.ppEnabledExtensionNames = DeviceExtensionName.data()
		};

		return m_PhysicalDevice.createDevice(DeviceInfo);
	}

	vk::PhysicalDevice& PhysicalDevice::getPhysicalDevice()
	{
		return m_PhysicalDevice;
	}

	vk::PhysicalDevice PhysicalDevice::cgetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	bool PhysicalDevice::addQueue(const vk::QueueFlagBits& RequiredQueue, const float& QueuePriority, const uint32_t& QueueCount)
	{
		// find queue
		auto QueueFamilies{ m_PhysicalDevice.getQueueFamilyProperties() };

		auto Iterator
		{
			std::find_if(QueueFamilies.cbegin(), QueueFamilies.cend(),
			[RequiredQueue, QueueCount](const vk::QueueFamilyProperties& QFP)
			{
				return (QFP.queueFlags & RequiredQueue) && (QFP.queueCount >= QueueCount);

			})
		};

		if (Iterator == QueueFamilies.cend()){ return false; }

		auto Index{ static_cast<uint32_t>(std::distance(QueueFamilies.cbegin(), Iterator)) };

		// check if graphics queue, special case
		if (RequiredQueue & vk::QueueFlagBits::eGraphics)
		{
			m_GraphicsQueue = vk::DeviceQueueCreateInfo
			{
				.queueFamilyIndex = Index,
				.queueCount = QueueFamilies.at(Index).queueCount,
				.pQueuePriorities = &QueuePriority
			};

			return true;
		}

		// save queue info
		m_DeviceQueues.push_back(
			vk::DeviceQueueCreateInfo
			{
				.queueFamilyIndex = Index,
				.queueCount = QueueFamilies.at(Index).queueCount,
				.pQueuePriorities = &QueuePriority
			}
		);

		return true;
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


	bool PhysicalDevice::findPresentQueue(const vk::SurfaceKHR& Surface, const float& PresentQPriority, const uint32_t& MinPresentQCount)
	{
		assert(m_GraphicsQueue.has_value());

		if (m_PhysicalDevice.getSurfaceSupportKHR(m_GraphicsQueue.value().queueFamilyIndex, Surface))
		{
			m_GraphicsCanPresent = true;
			return true;
		}

		m_GraphicsCanPresent = findGraphicsQueueWithPresent(Surface, 1.f, 1, PresentQPriority, MinPresentQCount);

		return m_GraphicsCanPresent;
	}

	bool PhysicalDevice::findGraphicsQueueWithPresent(
		const vk::SurfaceKHR& Surface,
		const float& GraphicsQPriority, const uint32_t& MinGraphicsQCount,
		const float& PresentQPriority, const uint32_t& MinPresentQCount)
	{
		if(!m_GraphicsQueue.has_value())
		{
			if(!addQueue(vk::QueueFlagBits::eGraphics, GraphicsQPriority, MinGraphicsQCount))
			{
				return false;
			}
		}

		// if graphics queue exist and supports present
		if (m_PhysicalDevice.getSurfaceSupportKHR(m_GraphicsQueue.value().queueFamilyIndex, Surface))
		{
			m_PresentQueue = m_GraphicsQueue.value().queueFamilyIndex;
			return true;
		}

		// else
		auto QueueFamilies{ m_PhysicalDevice.getQueueFamilyProperties() };

		for (auto Index = 0; Index < QueueFamilies.size(); Index++)
		{
			if (m_PhysicalDevice.getSurfaceSupportKHR(Index, Surface))
			{
				m_PresentQueue = Index;

				// create seperate presentation queue
				m_DeviceQueues.push_back(
					{
						.queueFamilyIndex = static_cast<uint32_t>(Index),
						.queueCount = MinPresentQCount,
						.pQueuePriorities = &PresentQPriority
					});
				return false;
			}
		}

		throw std::runtime_error("Cannot find present queue");
	}

	std::array<uint32_t, 2> PhysicalDevice::getGraphicsPresentQueueIndices() const
	{
		return { { m_GraphicsQueue->queueFamilyIndex, m_PresentQueue } };
	}

	bool PhysicalDevice::GraphicsCanPresent() const
	{
		return m_GraphicsCanPresent;
	}


	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	bool PhysicalDevice::extensionSupport(const vk::PhysicalDevice& PhysicalDevice, const std::vector<const char*>& RequiredExtensions)
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
}
