module;
#include <vulkan/vulkan.hpp>
#include <vector>
#include <utility>
#include <cstdint>

export module VT.Platform.Vulkan.PhysicalDevice;

export namespace VT::Vulkan
{
class PhysicalDevice
{
public:
    vk::Device CreateLogicalDevice(const std::vector<const char*>& DeviceExtensionName = {}) const;

    /* Add queues needed by logical device
     * must be added before creating the logical device
     */
    bool AddQueue(vk::QueueFlagBits RequiredQueue, float QueuePriority = 0.f, uint32_t QueueCount = 1);

    bool FindPhysicalDevice(
        const std::vector<vk::PhysicalDevice>& DeviceList,
        const std::vector<vk::PhysicalDeviceProperties>& DeviceProperties,
        const std::vector<const char*>& DeviceRequiredExtensions);

    /*
     * Determines if extra device extensions will be required
     * DeviceRequiredExtensions will be modified if VK_KHR_portability_subset is supported
     *   The Vulkan spec states: If the VK_KHR_portability_subset extension is included in pProperties of
     * vkEnumerateDeviceExtensionProperties, ppEnabledExtensionNames must include "VK_KHR_portability_subset"
     *   (https://vulkan.lunarg.com/doc/view/1.3.275.0/mac/1.3-extensions/vkspec.html#VUID-VkDeviceCreateInfo-pProperties-04451)
     */

    bool FindPresentQueue(vk::SurfaceKHR Surface);

    bool FindGraphicsQueueWithPresent(vk::SurfaceKHR Surface, float GraphicsQPriority, uint32_t MinGraphicsQCount);

    bool SupportsPortabilitySubset() const;
    bool GraphicsQueueCanPresent() const;
    /**
     * get Device queues
     */
    std::vector<std::pair<vk::QueueFlagBits, vk::DeviceQueueCreateInfo>> GetDeviceQueues() const;

    vk::PhysicalDevice GetPhysicalDevice() const;
    vk::DeviceQueueCreateInfo GetPresentQueue() const;
    vk::DeviceQueueCreateInfo GetGraphicsQueue() const;

    /* Resets all states inside physical device
     * All states except for the physical device itself will be erased
     */
    void Rest();

    PhysicalDevice(
        const std::vector<vk::PhysicalDevice>& DeviceList,
        const std::vector<vk::PhysicalDeviceProperties>& DeviceProperties,
        const std::vector<const char*>& DeviceRequiredExtensions,
        const char* Name);
    PhysicalDevice()                          = default;
    PhysicalDevice(PhysicalDevice&)           = delete;
    PhysicalDevice& operator=(PhysicalDevice) = delete;

private:
    static bool ExtensionSupported(
        const vk::PhysicalDevice& PhysicalDevice,
        const std::vector<const char*>& RequiredExtensions);

private:
    // <Purpose of queue, queue info>
    // could all be referring to the same queue
    std::vector<std::pair<vk::QueueFlagBits, vk::DeviceQueueCreateInfo>> m_DeviceQueues;

    vk::PhysicalDevice m_PhysicalDevice;

    int m_PresentQueue {-1};
    int m_GraphicsQueue {-1};
};
} // namespace VT::Vulkan
