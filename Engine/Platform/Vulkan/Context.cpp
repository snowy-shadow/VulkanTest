module;
#include <GLFW/glfw3.h>

#include <compare>
#include <vulkan/vulkan.hpp>

#include "EngineMacro.h"

module VT.Platform.Vulkan.Context;
import VT.Log;
import VT.Util;

#define VK_CHECK(res, expect, ...)              \
    VT_CORE_ASSERT(                             \
        static_cast<vk::Result>(res) == expect, \
        "Vulkan check failed at ",              \
        __FILE__,                               \
        __LINE__,                               \
        " : ",                                  \
        __VA_ARGS__);

namespace VT::Vulkan
{
Context::Context(Shared<Window> Window) : m_Window(Window) {}

void Context::Init()
{
    VT_CORE_TRACE("Vulkan context init");
    /* ===============================================
     *          Create Vulkan Instance
     * ===============================================
     */
    {
        std::vector<const char*> InstanceExtension;
        std::vector<const char*> InstanceLayer;

        switch (m_Window->GetWindowAPI())
        {
            case WindowAPI::eGLFWwindow:
            {
                VT_CORE_ASSERT(glfwVulkanSupported(), "GLFW Window context does not support vulkan");
                uint32_t GlfwExtensionCount;
                const char** GlfwExtension = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);

                // append to extension list
                InstanceExtension.reserve(GlfwExtensionCount);
                InstanceExtension.insert(InstanceExtension.cend(), GlfwExtension, GlfwExtension + GlfwExtensionCount);

                break;
            }
        }

        m_Instance.Init({.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0)}, InstanceExtension, InstanceLayer);
    }

    VT_CORE_TRACE("Vulkan instance init");
    /* ===============================================
     *          Variables
     * ===============================================
     */
    std::vector<const char*> DeviceExtension {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    vk::Instance VulkanInstance = m_Instance.GetInstance();
    /* ===============================================
     *          Create Physical device
     * ===============================================
     */
    {
        const auto PDs = VulkanInstance.enumeratePhysicalDevices();
        std::vector<vk::PhysicalDeviceProperties> PDProperties;
        PDProperties.reserve(PDs.size());

        for (const auto& PD : PDs)
        {
            PDProperties.emplace_back(PD.getProperties());
        }

        VT_CORE_ASSERT(
            m_PhysicalDevice.FindPhysicalDevice(PDs, PDProperties, DeviceExtension),
            "Cannot find compatible physical device");
    }

    VT_CORE_TRACE("Vulkan Physical device created");
    /* ===============================================
     *          Create Window Surface
     * ===============================================
     */
    switch (m_Window->GetWindowAPI())
    {
        case WindowAPI::eGLFWwindow:
        {
            VK_CHECK(
                glfwCreateWindowSurface(
                    VulkanInstance,
                    static_cast<GLFWwindow*>(m_Window->GetNativeWindow()),
                    nullptr,
                    reinterpret_cast<VkSurfaceKHR*>(&m_Surface)),
                vk::Result::eSuccess,
                "Failed to create window surface");
            break;
        }
    }

    VT_CORE_TRACE("Vulkan window surface created");
    /* ===============================================
     *          Create Logical device
     * ===============================================
     */
    {
        // add queue
        // m_PhysicalDevice.addQueue(vk::QueueFlagBits::eGraphics, 1.f);
        // m_PhysicalDevice.addQueue(vk::QueueFlagBits::eTransfer, 1.f);
        VT_CORE_ASSERT(
            m_PhysicalDevice.FindGraphicsQueueWithPresent(m_Surface, 1.f, 1),
            "Failed to find graphics queue that can present");

        if (m_PhysicalDevice.SupportsPortabilitySubset())
        {
            DeviceExtension.emplace_back("VK_KHR_portability_subset");
        }

        // create device;
        m_LogicalDevice = m_PhysicalDevice.CreateLogicalDevice(DeviceExtension);
    }

    VT_CORE_TRACE("Vulkan logical device created");
    /* ===============================================
     *          Create Command Pool
     * ===============================================
     */
    {
        m_CmdPool = m_LogicalDevice.createCommandPool(
            {.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
             .queueFamilyIndex = m_PhysicalDevice.GetGraphicsQueue().queueFamilyIndex});
    }
}

void Context::SwapBuffers() {}

Context::~Context() { m_LogicalDevice.destroyCommandPool(m_CmdPool); }

} // namespace VT::Vulkan
