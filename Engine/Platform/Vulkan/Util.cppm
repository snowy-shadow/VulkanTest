module;
#include "Vulkan.h"

export module VT.Platform.Vulkan.Util;
import VT.Log;

export namespace VT::Vulkan
{
std::pair<bool, uint32_t> FindMemoryTypeIndex(
    const vk::PhysicalDeviceMemoryProperties& MemProperties,
    uint32_t TypeFilter,
    vk::MemoryPropertyFlags Properties) noexcept
{
    /*
    2 arrays:
        memory type: VRAM, swap in RAM, etc
        memory heap: where the mem comes from
    */

    for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
    {
        if ((TypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
        {
            return {true, i};
        }
    }

    VT_CORE_HALT("Failed to find suitable memory type!");
    return {false, -1};
}
}

