module;
#include <vulkan/vulkan.hpp>
#include <vector>

export module VT.Platform.Vulkan.Instance;
import VT.Log;
import VT.Util;

export namespace VT::Vulkan
{
class Instance
{
public:
    /*
     * Create vulkan instance
     */
    bool Init(
        const vk::ApplicationInfo& ApplicationInfo,
        std::vector<const char*> RequiredExtensions,
        std::vector<const char*> RequiredLayers);

    vk::Instance GetInstance() const noexcept;

    Instance() = default;
    ~Instance();
    Instance(Instance&)            = delete;
    Instance& operator=(Instance&) = delete;

public:

private:
    bool static IsSupported(std::span<const char*> RequiredExtensions = {}, std::span<const char*> RequiredLayers = {});

private:
    vk::Instance m_VulkanInstance;

#ifndef NDEBUG
    // Keep logger alive until debugger destroyed
    Shared<Log> m_Logger {Log::Instance()};
    vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
};
} // namespace VT::Vulkan
