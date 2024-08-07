module;
#include <vulkan/vulkan.hpp>

#include <utility>
export module VT.Platform.Vulkan.Native.Swapchain;

import VT.Platform.Vulkan.Native.PhysicalDevice;

export namespace VT::Vulkan::Native
{
class Swapchain
{
public:
    struct Capabilities
    {
        uint32_t minImageCount;
        vk::Extent2D imageExtent;
        uint32_t arrayLayers;
        std::vector<vk::SurfaceTransformFlagBitsKHR> surfaceTransform;
        std::vector<vk::CompositeAlphaFlagBitsKHR> compositeAlpha;
    };

public:
    void Init(vk::Device);
    /*
     * Finds and replaces the first supported format for each in vk::SwapchainCreateInfoKHR.
     */
    std::pair<bool, vk::SwapchainCreateInfoKHR> QueryCapabilities(
        vk::SwapchainCreateInfoKHR Createinfo,
        Capabilities PreferredCapabilities,
        vk::PhysicalDevice PhysicalDevice,
        vk::SurfaceKHR Surface) const;

    std::pair<bool, vk::SurfaceFormatKHR> FindSurfaceFormat(
        const std::vector<vk::SurfaceFormatKHR>& Preferred,
        const std::vector<vk::SurfaceFormatKHR>& SupportedSurfaceFormats) const;

    std::pair<bool, vk::PresentModeKHR> FindPresentMode(
        const std::vector<vk::PresentModeKHR>& Preferred,
        const std::vector<vk::PresentModeKHR>& SupportedPresentMode) const;

    void CreateSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice);

    /**
     * replace current swapchain with new one. sets SwapchainCreateInfo.oldSwapchin = current swapchain
     * @param SwapchainCreateInfo : create info
     * @param LogicalDevice : Logical device used to create swapchain
     */
    void RecreateSwapchain(vk::SwapchainCreateInfoKHR SwapchainCreateInfo, vk::Device LogicalDevice);

    vk::SwapchainKHR Get() const noexcept;
    vk::SwapchainCreateInfoKHR GetInfo() const noexcept;

public:
    Swapchain() = default;
    Swapchain(vk::Device);
    Swapchain(Swapchain&& Other) noexcept;
    Swapchain& operator=(Swapchain&& Other) noexcept;
    Swapchain operator=(Swapchain&) = delete;
    Swapchain(const Swapchain&)     = delete;

    void Destroy();
    ~Swapchain();

private:
    vk::Device m_LogicalDevice;

    vk::SwapchainCreateInfoKHR m_SwapchainCreateInfo;
    vk::SwapchainKHR m_Swapchain;

    bool m_SwapchainCreated {false};
};
} // namespace VT::Vulkan::Native
