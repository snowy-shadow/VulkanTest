module;

#include <vector>
#include <string>
#include <unordered_set>
#include <vulkan/vulkan.hpp>
#include "EngineMacro.h"

module VT.Platform.Vulkan.Instance;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

import VT.Log;

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

namespace VT::Vulkan
{
VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT, // MessageSeverity
    VkDebugUtilsMessageTypeFlagsEXT,        // MessageType
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    reinterpret_cast<Log*>(pUserData)->CoreLogger->error(
        "Message ID : {0}\n Message : {1}",
        pCallbackData->pMessageIdName,
        pCallbackData->pMessage);

    return vk::False;
}
} // namespace VT::Vulkan
#endif

/* ======================================================
 *              Instance
 *  ======================================================
 */
namespace VT::Vulkan
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
    VT_CORE_ASSERT(ApplicationInfo.apiVersion <= vk::enumerateInstanceVersion(), "Required api version too high!");

    /* ======================================================
     *              Extensions and Layer config
     *  ======================================================
     */
#ifdef VT_ENABLE_DEBUG
    Layers.emplace_back("VK_LAYER_KHRONOS_validation");
    Extensions.emplace_back("VK_EXT_debug_utils");
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

    m_VulkanInstance = vk::createInstance(InstanceCreateInfo, nullptr);

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

    m_DebugMessenger = m_VulkanInstance.createDebugUtilsMessengerEXT(DebugMessengerCreateInfo);

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
    // extension
    auto InstanceExtensions {vk::enumerateInstanceExtensionProperties()};
    std::unordered_set<std::string> Capability(std::max(RequiredExtensions.size(), RequiredLayers.size()) * 2);

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

    // layer
    Capability.clear();
    auto InstanceLayerProperties {vk::enumerateInstanceLayerProperties()};
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

    // if instance supports layers and extensions
    return true;
}

} // namespace VT::Vulkan
