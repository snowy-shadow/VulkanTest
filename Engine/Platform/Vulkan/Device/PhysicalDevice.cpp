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

vk::Device PhysicalDevice::CreateLogicalDevice(const std::vector<const char*>& DeviceExtensionName) const
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
        .ppEnabledExtensionNames = DeviceExtensionName.data()};

    return m_PhysicalDevice.createDevice(DeviceInfo);
}

bool PhysicalDevice::AddQueue(vk::QueueFlagBits RequiredQueue, float QueuePriority, uint32_t QueueCount)
{
    // find queue
    auto QueueFamilies {m_PhysicalDevice.getQueueFamilyProperties()};

    auto Iterator = std::find_if(
        QueueFamilies.cbegin(),
        QueueFamilies.cend(),
        [&RequiredQueue, &QueueCount](const vk::QueueFamilyProperties& QFP)
        { return (QFP.queueFlags & RequiredQueue) && (QFP.queueCount >= QueueCount); });

    if (Iterator == QueueFamilies.cend())
    {
        return false;
    }

    auto Index {static_cast<uint32_t>(std::distance(QueueFamilies.cbegin(), Iterator))};

    // save queue info
    m_DeviceQueues.emplace_back(
        RequiredQueue,
        vk::DeviceQueueCreateInfo {
            .queueFamilyIndex = Index,
            .queueCount       = QueueFamilies.at(Index).queueCount,
            .pQueuePriorities = &QueuePriority});

    if (RequiredQueue == vk::QueueFlagBits::eGraphics)
    {
        m_GraphicsQueue = static_cast<int>(m_DeviceQueues.size() - 1);
    }
    return true;
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
        m_PhysicalDevice           = DeviceList[DiscreteGPUs.front()];
        m_PhysicalDeviceProperties = DeviceProperties[DiscreteGPUs.front()];
    }
    else
    {
        m_PhysicalDevice           = DeviceList[BasicGPUs.front()];
        m_PhysicalDeviceProperties = DeviceProperties[BasicGPUs.front()];
    }
    return true;
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

bool PhysicalDevice::FindGraphicsQueueWithPresent(
    vk::SurfaceKHR Surface,
    float GraphicsQPriority,
    uint32_t MinGraphicsQCount)
{
    // find queue
    auto QueueFamilies {m_PhysicalDevice.getQueueFamilyProperties()};

    for (int Index = 0; Index < QueueFamilies.size(); Index++)
    {
        auto& QF = QueueFamilies[Index];

        if ((QF.queueFlags & vk::QueueFlagBits::eGraphics) && (QF.queueCount >= MinGraphicsQCount) &&
            m_PhysicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(Index), Surface))
        {
            auto Iter = m_DeviceQueues.emplace_back(
                vk::QueueFlagBits::eGraphics,
                vk::DeviceQueueCreateInfo {
                    .queueFamilyIndex = static_cast<uint32_t>(Index),
                    .queueCount       = MinGraphicsQCount,
                    .pQueuePriorities = &GraphicsQPriority});

            // last element index
            m_GraphicsQueue = m_PresentQueue = static_cast<int>(m_DeviceQueues.size() - 1);
            return true;
        }
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

    VT_CORE_HALT("Cannot find present queue, try adding more queues");
#ifndef VT_CORE_ASSERT
    return false;
#endif
}

bool PhysicalDevice::GraphicsQueueCanPresent() const { return m_GraphicsQueue == m_PresentQueue; }

std::vector<std::pair<vk::QueueFlagBits, vk::DeviceQueueCreateInfo>> PhysicalDevice::GetDeviceQueues() const
{
    return m_DeviceQueues;
}

vk::DeviceQueueCreateInfo PhysicalDevice::GetPresentQueue() const { return m_DeviceQueues[m_PresentQueue].second; }
vk::DeviceQueueCreateInfo PhysicalDevice::GetGraphicsQueue() const { return m_DeviceQueues[m_GraphicsQueue].second; }

vk::PhysicalDevice PhysicalDevice::GetPhysicalDevice() const { return m_PhysicalDevice; }

void PhysicalDevice::Rest() { m_DeviceQueues.clear(); }
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
