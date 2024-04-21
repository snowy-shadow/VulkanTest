#include "Device.h"
#include <ranges>

namespace VT
{
	void Device::bindInstance(vk::Instance VulkanInstance) { m_VulkanInstance = VulkanInstance; }

	void Device::createDevice(std::string Name, GLFWwindow* Window, const std::vector<const char*>& RequiredExtensions, const std::vector<std::tuple<vk::QueueFlagBits, float>>& RequiredQueues)
	{
		// m_VulkanInstance created
		assert(m_VulkanInstance);
		
		if (static_cast<vk::Result>(glfwCreateWindowSurface(m_VulkanInstance, Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface))) != vk::Result::eSuccess)
		{
			throw std::runtime_error("Failed to create window surface");
		}

		m_ObjectConstructedMask |= ObjectConstructed::eSurface;
	
		// find device
		if (!m_PhysicalDevice.findPhysicalDevice(m_VulkanInstance.enumeratePhysicalDevices(), RequiredExtensions)) { throw std::runtime_error("Cannot find compatible physical device"); }

		// add queue
		for (const auto& Q : RequiredQueues)
		{
			m_PhysicalDevice.addQueue(std::get<vk::QueueFlagBits>(Q), std::get<float>(Q));
		}

		// find present queue
		if (!m_PhysicalDevice.findPresentQueue(m_Surface, 1.f, 1)) { throw std::runtime_error("Did not find present queue"); }

        auto Extensions = RequiredExtensions;
        if(m_PhysicalDevice.supportsPortabilitySubset()) { Extensions.emplace_back("VK_KHR_portability_subset"); }
		// create device;
		auto [_, Result] = m_LogicalDevices.try_emplace(Name, m_PhysicalDevice.createLogicalDevice(Extensions));

		if (!Result) { throw std::runtime_error("Logical Device name collision! Attempt to create logical device with name : " + std::move(Name)); }
	}

	std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Device::getDeviceReferences(std::string Name)
	{
		auto LD_Iter = m_LogicalDevices.find(Name);
		if(LD_Iter == m_LogicalDevices.cend()) { throw std::runtime_error("No logical device with name : " + std::move(Name)); }
		
		return std::tuple{ LD_Iter->second, &m_PhysicalDevice, m_Surface };
	}

	Device::~Device()
	{
		if (m_ObjectConstructedMask & ObjectConstructed::eSurface) { m_VulkanInstance.destroySurfaceKHR(m_Surface); }

		for(auto& D : m_LogicalDevices | std::views::values) { D.destroy(); }
	}
}
