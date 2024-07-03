module;
#include <vulkan/vulkan.hpp>

export module VT.Platform.Vulkan.Device;

export namespace VT::Vulkan
{
class LogicalDevice
{
public:
    void Init(vk::Device LogicalDevice) { m_LogicalDevice = LogicalDevice; }

    vk::Device Get() const { return m_LogicalDevice; }

public:
    LogicalDevice()                                 = default;
    LogicalDevice(const LogicalDevice&)             = delete;
    LogicalDevice(const LogicalDevice&&)            = delete;
    LogicalDevice& operator=(const LogicalDevice&)  = delete;
    LogicalDevice& operator=(const LogicalDevice&&) = delete;
    ~LogicalDevice() { m_LogicalDevice.destroy(); }

private:
    vk::Device m_LogicalDevice;
};

class Surface
{
public:
    void Init(vk::Instance Instance, vk::SurfaceKHR Surface)
    {
        m_Instance = Instance;
        m_Surface  = Surface;
    }

    vk::SurfaceKHR Get() const { return m_Surface; }

public:
    Surface()                           = default;
    Surface(const Surface&)             = delete;
    Surface(const Surface&&)            = delete;
    Surface& operator=(const Surface&)  = delete;
    Surface& operator=(const Surface&&) = delete;
    ~Surface() { m_Instance.destroySurfaceKHR(m_Surface); }

private:
    vk::Instance m_Instance;
    vk::SurfaceKHR m_Surface;
};
} // namespace VT::Vulkan
