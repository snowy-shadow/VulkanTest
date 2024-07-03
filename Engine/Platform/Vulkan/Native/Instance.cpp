module;
#include <vector>
#include <string>
#include <unordered_set>
#include "Platform/Vulkan/Vulkan.h"

module VT.Platform.Vulkan.Native.Instance;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

#ifdef VT_ENABLE_DEBUG
/* ======================================================
 *              DebugMessenger
 *  ======================================================
 */
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance Instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger)
{
    auto pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT"));

    VT_CORE_ASSERT(
        pfnVkCreateDebugUtilsMessengerEXT,
        "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.");

    return pfnVkCreateDebugUtilsMessengerEXT(Instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance Instance,
    VkDebugUtilsMessengerEXT Messenger,
    VkAllocationCallbacks const* pAllocator)
{
    auto pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT"));

    VT_CORE_ASSERT(
        pfnVkDestroyDebugUtilsMessengerEXT,
        "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.");

    return pfnVkDestroyDebugUtilsMessengerEXT(Instance, Messenger, pAllocator);
}

namespace VT::Vulkan::Native
{
VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT,
    // MessageSeverity
    VkDebugUtilsMessageTypeFlagsEXT,
    // MessageType
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    reinterpret_cast<Log*>(pUserData)->CoreLogger->error(
        "Message ID : {0}\n Message : {1}",
        pCallbackData->pMessageIdName,
        pCallbackData->pMessage);

    return vk::False;
}
} // namespace VT::Vulkan::Native
#endif

/* ======================================================
 *              Instance
 *  ======================================================
 */
namespace VT::Vulkan::Native
{

bool Instance::Init(
    const vk::ApplicationInfo& ApplicationInfo,
    std::vector<const char*> Extensions,
    std::vector<const char*> Layers)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    /* ======================================================
     *              API version check
     *  ======================================================
     */
    auto [Result, InstanceVersion] = vk::enumerateInstanceVersion();
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to get instance version");

    VT_CORE_ASSERT(ApplicationInfo.apiVersion <= InstanceVersion, "Required api version too high!");

/* ======================================================
 *              Extensions and Layer config
 *  ======================================================
 */
#ifdef VT_ENABLE_DEBUG
    Layers.emplace_back("VK_LAYER_KHRONOS_validation");
    Extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef __APPLE__
    Extensions.emplace_back("VK_KHR_portability_enumeration");
#endif

    if (!IsSupported(Extensions, Layers))
    {
        VT_CORE_HALT("Instance does not support required extensions and layers");
        return false;
    }

    /* ======================================================
     *              Create Vulkan instance
     *  ======================================================
     */
    const vk::InstanceCreateInfo InstanceCreateInfo {
#ifdef __APPLE__
        .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
#endif
        .pApplicationInfo        = &ApplicationInfo,
        .enabledLayerCount       = static_cast<uint32_t>(Layers.size()),
        .ppEnabledLayerNames     = Layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(Extensions.size()),
        .ppEnabledExtensionNames = Extensions.data()};

    std::tie(Result, m_VulkanInstance) = vk::createInstance(InstanceCreateInfo, nullptr);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create instance");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanInstance);
    VT_CORE_TRACE("Instance created");

/* ======================================================
 *              Validation Layer
 *  ======================================================
 */
#ifdef VT_ENABLE_DEBUG

    vk::DebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo {
        .messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = DebugCallback,
        .pUserData       = m_Logger.get()};

    std::tie(Result, m_DebugMessenger) = m_VulkanInstance.createDebugUtilsMessengerEXT(DebugMessengerCreateInfo);
    VK_CHECK(Result, vk::Result::eSuccess, "Failed to create Debug util messenger");

    VT_CORE_TRACE("Validation layer enabled");
#endif
}

vk::Instance Instance::Get() const noexcept { return m_VulkanInstance; }

Instance::~Instance()
{

#ifdef VT_ENABLE_DEBUG
    if (m_DebugMessenger != VK_NULL_HANDLE)
    {
        m_VulkanInstance.destroyDebugUtilsMessengerEXT(m_DebugMessenger);
    }
#endif

    if (m_VulkanInstance != VK_NULL_HANDLE)
    {
        m_VulkanInstance.destroy();
    }
}

/* ==========================================
 *				  Private
 * ==========================================
 */
bool Instance::IsSupported(std::span<const char*> RequiredExtensions, std::span<const char*> RequiredLayers)
{
    std::unordered_set<std::string> Capability(std::max(RequiredExtensions.size(), RequiredLayers.size()) * 2);
    {
        // extension
        const auto [Result, InstanceExtensions] {vk::enumerateInstanceExtensionProperties()};
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to enumerate instance extension properties");

        for (const auto& Ext : InstanceExtensions)
        {
            Capability.insert(Ext.extensionName);
        }
        // if support extensions
        for (const auto& ReqExt : RequiredExtensions)
        {
            if (!Capability.contains(ReqExt))
            {
                return false;
            }
        }
    }
    {
        // layer
        Capability.clear();
        const auto [Result, InstanceLayerProperties] {vk::enumerateInstanceLayerProperties()};
        VK_CHECK(Result, vk::Result::eSuccess, "Failed to enumerate instance layer properties");

        for (const auto& Layer : InstanceLayerProperties)
        {
            Capability.insert(Layer.layerName);
        }

        // if support layers
        for (const auto& ReqLayer : RequiredLayers)
        {
            if (!Capability.contains(ReqLayer))
            {
                return false;
            }
        }
    }

    // if instance supports layers and extensions
    return true;
}

} // namespace VT::Vulkan::Native