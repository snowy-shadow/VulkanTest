module;
#include <vulkan/vulkan.hpp>

export module VT.Platform.Vulkan.Device;

export namespace VT::Vulkan
{
class LogicalDevice
{
public:
    void Init(vk::Device LogicalDevice) { m_LogicalDevice = LogicalDevice; }

    vk::Device Get() { return m_LogicalDevice; }

    ~LogicalDevice() { m_LogicalDevice.destroy(); }

private:
    vk::Device m_LogicalDevice;
};
} // namespace VT::Vulkan
