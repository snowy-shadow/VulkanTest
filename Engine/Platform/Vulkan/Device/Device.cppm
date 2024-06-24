module;
#include <vulkan/vulkan.hpp>
export module VT.Platform.Vulkan.Device;
import VT.Platform.Vulkan.PhysicalDevice;
import VT.Util;
import VT.Window;

export namespace VT::Vulkan
{
class Device
{
public:
    void bindInstance(vk::Instance VulkanInstance);
    /*
     * Creates Physical and Logical device with given Name
     * Name - PhysicalDevice Name, LogicalDevice Name
     * Window - glfw window handle
     * RequiredExtensions - Physical device extensions
     * RequiredQueues - Physical device queues
     */
    void createDevice(
        vk::Instance VulkanInstance,
        vk::SurfaceKHR Surface,
        Shared<Window> Window,
        const std::vector<const char*>& RequiredExtensions,
        const std::vector<std::pair<vk::QueueFlagBits, float>>& RequiredQueues);

    /*
     * Device - LogicalDevice Name
     */
    vk::Device GetDevice() const { return m_LogicalDevice; }

    ~Device();

private:
    vk::Device m_LogicalDevice;
};
} // namespace VT::Vulkan
