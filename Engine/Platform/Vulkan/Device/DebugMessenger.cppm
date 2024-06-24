module;
#include <vulkan/vulkan.hpp>
#include "EngineMacro.h"
export module VT.Platform.Vulkan.Device.DebugMessenger;

import VT.Log;

namespace VT
{
VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT, // MessageSeverity
    VkDebugUtilsMessageTypeFlagsEXT,        // MessageType
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* // pUserData
)
{
    VT_CORE_ERROR("Message ID : {0}\n Message : {1}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
    return vk::False;
}

vk::DebugUtilsMessengerEXT createDebugMessenger(
    const vk::Instance& Instance,
    const vk::DispatchLoaderDynamic& DLD,
    vk::DebugUtilsMessageSeverityFlagsEXT MessageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT MessageType)
{
    vk::DebugUtilsMessengerCreateInfoEXT CreateInfo {
        .messageSeverity = MessageSeverity,
        .messageType     = MessageType,
        .pfnUserCallback = debugCallback};

    return Instance.createDebugUtilsMessengerEXT(CreateInfo, nullptr, DLD);
}

} // namespace VT
