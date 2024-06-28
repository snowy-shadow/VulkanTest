module;
#include <unordered_set>
#include <array>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <utility>
#include "EngineMacro.h"

module VT.Platform.Vulkan.PhysicalDevice;
import VT.Log;

namespace VT::Vulkan
{
PhysicalDevice::PhysicalDevice(
    const std::vector<vk::PhysicalDevice>& DeviceList,
    const std::vector<vk::PhysicalDeviceProperties>& DeviceProperties,
    const std::vector<const char*>& RequiredExtensions,
    const char* Name)
{
    for (int i = 0; i < DeviceList.size(); i++)
    {
        if (ExtensionSupported(DeviceList[i], RequiredExtensions) && strcmp(DeviceProperties[i].deviceName, Name) == 0)
        {
            m_PhysicalDevice = DeviceList[i];
        }
    }

    VT_CORE_HALT("Device name not found");
}

bool PhysicalDevice::FindPhysicalDevice(
    const std::vector<vk::PhysicalDevice>& DeviceList,
    const std::vector<vk::PhysicalDeviceProperties>& DeviceProperties,
    const std::vector<const char*>& RequiredExtensions)
{
    // list of gpu meeting extension criteria (index)
    std::vector<uint32_t> BasicGPUs;
    // list of discrete gpu meeting extension criteria (index)
    std::vector<uint32_t> DiscreteGPUs;

    // DiscreteGPU is a subset of BasicGPU
    BasicGPUs.reserve(DeviceList.size());
    DiscreteGPUs.reserve(DeviceList.size());

    for (int i = 0; i < DeviceList.size(); i++)
    {
        if (ExtensionSupported(DeviceList[i], RequiredExtensions))
        {
            if (DeviceProperties[i].deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                DiscreteGPUs.push_back(i);
            }

            BasicGPUs.push_back(i);
        }
    }

    // no gpu
    if (BasicGPUs.size() == 0)
    {
        VT_CORE_HALT("Unable to find appropriate physical device");
        return false;
    }

    if (DiscreteGPUs.size() != 0)
    {
        m_PhysicalDevice = DeviceList[DiscreteGPUs.front()];
    }
    else
    {
        m_PhysicalDevice = DeviceList[BasicGPUs.front()];
    }
    return true;
}
bool PhysicalDevice::AddQueue(vk::QueueFlagBits RequiredQueue, std::span<const float> QueuePriorities)
{
    // find queue
    auto QueueFamilies {m_PhysicalDevice.getQueueFamilyProperties()};

    auto Iterator = std::find_if(
        QueueFamilies.cbegin(),
        QueueFamilies.cend(),
        [RequiredQueue, QueueCount = QueuePriorities.size()](const vk::QueueFamilyProperties& QFP)
        { return (QFP.queueFlags & RequiredQueue) && (QFP.queueCount >= QueueCount); });

    if (Iterator == QueueFamilies.cend())
    {
        return false;
    }

    auto Index {static_cast<uint32_t>(std::distance(QueueFamilies.cbegin(), Iterator))};

    // save queue info
    auto& DeviceQ = m_DeviceQueues.emplace_back(
        RequiredQueue,
        vk::DeviceQueueCreateInfo {
            .queueFamilyIndex = static_cast<uint32_t>(Index),
            .queueCount       = static_cast<uint32_t>(QueuePriorities.size()),
            .pQueuePriorities = QueuePriorities.data()});

    if (RequiredQueue == vk::QueueFlagBits::eGraphics)
    {
        m_GraphicsQueue = static_cast<int>(&DeviceQ - &(*m_DeviceQueues.cbegin()));
    }
    return true;
}

bool PhysicalDevice::FindGraphicsQueueWithPresent(vk::SurfaceKHR Surface, std::span<const float> QueuePriorities)
{
    // find queue
    auto QueueFamilies {m_PhysicalDevice.getQueueFamilyProperties()};

    for (int Index = 0; Index < QueueFamilies.size(); Index++)
    {
        auto& QF = QueueFamilies[Index];

        bool Result = (QF.queueFlags & vk::QueueFlagBits::eGraphics) && (QF.queueCount >= QueuePriorities.size()) &&
                      m_PhysicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(Index), Surface);
        if (!Result)
        {
            continue;
        }

        auto& DeviceQ = m_DeviceQueues.emplace_back(
            vk::QueueFlagBits::eGraphics,
            vk::DeviceQueueCreateInfo {
                .queueFamilyIndex = static_cast<uint32_t>(Index),
                .queueCount       = static_cast<uint32_t>(QueuePriorities.size()),
                .pQueuePriorities = QueuePriorities.data()});

        // last element index
        m_GraphicsQueue = m_PresentQueue = static_cast<int>(&DeviceQ - &(*m_DeviceQueues.cbegin()));
        return true;
    }

    // FIX : try find a random queue that can present

    // else
    //  auto QueueFamilies {m_PhysicalDevice.getQueueFamilyProperties()};
    //
    //  for (std::uint32_t Index = 0; Index < QueueFamilies.size(); Index++)
    //  {
    //      if (m_PhysicalDevice.getSurfaceSupportKHR(Index, Surface))
    //      {
    //          m_PresentQueue = Index;
    //
    //          // create seperate presentation queue
    //          m_DeviceQueues.emplace_back(
    //              vk::QueueFlagBits::eCompute,
    //              {.queueFamilyIndex = static_cast<uint32_t>(Index),
    //               .queueCount       = MinPresentQCount,
    //               .pQueuePriorities = &PresentQPriority});
    //          return false;
    //      }
    //  }

    VT_CORE_ERROR("Cannot find present queue, try adding more queues");
    return false;
}

bool PhysicalDevice::SupportsPortabilitySubset() const
{
    VT_CORE_ASSERT(
        m_PhysicalDevice != VK_NULL_HANDLE,
        "Cannot check for protability subset if Physical device is not set first");

    auto Extensions {m_PhysicalDevice.enumerateDeviceLayerProperties()};

    for (auto Iter {Extensions.cbegin()}; Iter != Extensions.cend(); Iter++)
    {
        if (strcmp(Iter->layerName, "VK_KHR_portability_subset") == 0)
        {
            return true;
        }
    }

    return false;
}

bool PhysicalDevice::FindPresentQueue(vk::SurfaceKHR Surface)
{
    if (m_PresentQueue > -1)
    {
        return true;
    }

    for (int i = 0; i < m_DeviceQueues.size(); i++)
    {
        if (m_PhysicalDevice.getSurfaceSupportKHR(
                static_cast<uint32_t>(m_DeviceQueues[i].second.queueFamilyIndex),
                Surface))
        {
            m_PresentQueue = i;
            return true;
        }
    }

    return false;
}

vk::Device PhysicalDevice::CreateLogicalDevice(
    const std::vector<const char*>& DeviceExtensionName,
    vk::PhysicalDeviceFeatures EnableFeatures) const
{
    std::vector<vk::DeviceQueueCreateInfo> Queues;
    Queues.reserve(m_DeviceQueues.size());

    for (int i = 0; i < m_DeviceQueues.size(); i++)
    {
        Queues.push_back(m_DeviceQueues[i].second);
    }

    vk::DeviceCreateInfo DeviceInfo {
        .queueCreateInfoCount    = static_cast<uint32_t>(Queues.size()),
        .pQueueCreateInfos       = Queues.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(DeviceExtensionName.size()),
        .ppEnabledExtensionNames = DeviceExtensionName.data(),
        .pEnabledFeatures        = &EnableFeatures};

    return m_PhysicalDevice.createDevice(DeviceInfo);
}

uint32_t PhysicalDevice::FindMemoryType(uint32_t TypeFilter, vk::MemoryPropertyFlags Properties) const
{
    /*
    2 arrays:
        memory type: VRAM, swap in RAM, etc
        memory heap: where the mem comes from
    */
    vk::PhysicalDeviceMemoryProperties MemProperties = m_PhysicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
    {
        if ((TypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
        {
            return i;
        }
    }

    VT_CORE_HALT("failed to find suitable memory type");
    return (uint32_t) -1;
}

bool PhysicalDevice::GraphicsQueueCanPresent() const { return m_GraphicsQueue == m_PresentQueue; }

std::vector<std::pair<vk::QueueFlagBits, vk::DeviceQueueCreateInfo>> PhysicalDevice::GetDeviceQueues() const
{
    return m_DeviceQueues;
}

vk::DeviceQueueCreateInfo PhysicalDevice::GetPresentQueue() const { return m_DeviceQueues[m_PresentQueue].second; }
vk::DeviceQueueCreateInfo PhysicalDevice::GetGraphicsQueue() const { return m_DeviceQueues[m_GraphicsQueue].second; }

vk::PhysicalDevice PhysicalDevice::Get() const { return m_PhysicalDevice; }

void PhysicalDevice::Reset() { m_DeviceQueues.clear(); }
/*
 * ==================================================
 *					    PRIVATE
 * ==================================================
 */

bool PhysicalDevice::ExtensionSupported(
    const vk::PhysicalDevice& PhysicalDevice,
    const std::vector<const char*>& RequiredExtensions)
{
    // construct set
    std::unordered_set<std::string> DeviceSupportedExtentions;
    for (const auto& Extension : PhysicalDevice.enumerateDeviceExtensionProperties())
    {
        DeviceSupportedExtentions.insert(Extension.extensionName);
    }

    // check if set contains all required extensions
    for (const auto& Extension : RequiredExtensions)
    {
        if (!DeviceSupportedExtentions.contains(Extension))
        {
            return false;
        }
    }
    return true;
}
} // namespace VT::Vulkan
