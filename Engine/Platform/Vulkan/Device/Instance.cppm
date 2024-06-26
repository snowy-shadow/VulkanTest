module;
#include <vulkan/vulkan.hpp>
#include <vector>

export module VT.Platform.Vulkan.Instance;

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

private:
    bool static IsSupported(std::span<const char*> RequiredExtensions = {}, std::span<const char*> RequiredLayers = {});

private:
    vk::Instance m_VulkanInstance;

#ifndef NDEBUG
    vk::DispatchLoaderDynamic m_DLD_Instance;
    vk::DebugUtilsMessengerEXT m_DebugMessenger;
#endif
};
} // namespace VT::Vulkan
