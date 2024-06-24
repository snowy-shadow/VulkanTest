module;
#include "EngineMacro.h"

module VT.Platform.Vulkan.Device;
import VT.Platform.Vulkan.PhysicalDevice;
import VT.Window;

namespace VT::Vulkan
{
void Device::bindInstance(vk::Instance VulkanInstance) { m_VulkanInstance = VulkanInstance; }

void Device::createDevice(
    Window* Window,
    const std::vector<const char*>& RequiredExtensions,
    const std::vector<std::pair<vk::QueueFlagBits, float>>& RequiredQueues)
{
    // m_VulkanInstance created
    assert(m_VulkanInstance);

    if (static_cast<vk::Result>(glfwCreateWindowSurface(
            m_VulkanInstance, Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface))) != vk::Result::eSuccess)
    {
        VT_CORE_Halt("Failed to create window surface");
    }

    // find device
    if (!m_PhysicalDevice.findPhysicalDevice(m_VulkanInstance.enumeratePhysicalDevices(), RequiredExtensions))
    {
        VT_CORE_HALT("Cannot find compatible physical device");
    }

    // add queue
    for (const auto& Q : RequiredQueues)
    {
        m_PhysicalDevice.addQueue(Q.first, Q.second);
    }

    // find present queue
    if (!m_PhysicalDevice.findPresentQueue(m_Surface, 1.f, 1))
    {
        VT_CORE_HALT("Did not find present queue");
    }

    auto Extensions = RequiredExtensions;
    if (m_PhysicalDevice.supportsPortabilitySubset())
    {
        Extensions.emplace_back("VK_KHR_portability_subset");
    }

    // create device;
    m_LogicalDevices =  m_PhysicalDevice.createLogicalDevice(Extensions));
}

vk::Device GetDevice() const { return m_LogicalDevice; }

Device::~Device()
{
    if (m_Surface != VK_NULL_HANDLE)
    {
        m_VulkanInstance.destroySurfaceKHR(m_Surface);
    }

    m_LogicalDevice.destroy();
}
} // namespace VT::Vulkan
