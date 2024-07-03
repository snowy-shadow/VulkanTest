module;
#include <vulkan/vulkan.hpp>

export module VT.Platform.Vulkan.Swapchain;

import VT.Platform.Vulkan.Native.Instance;
import VT.Platform.Vulkan.Native.PhysicalDevice;
import VT.Platform.Vulkan.Native.Swapchain;
import VT.Platform.Vulkan.Attachment;
import VT.Window;

export namespace VT::Vulkan
{
class Swapchain
{
public:
	void Init(
		Window& Window,
		vk::Device LogicalDevice,
		Native::PhysicalDevice& PhysicalDevice,
		vk::SurfaceKHR Surface,
		uint32_t MaxFrameCount);

    std::pair<bool, uint32_t> AcquireNextImage(vk::Semaphore Semaphore);

	void Resize(uint32_t Width, uint32_t Height);

	vk::SwapchainKHR Get() const;
	vk::SwapchainCreateInfoKHR GetInfo() const;
    std::vector<std::vector<vk::ImageView>> GetImageView() const;
    uint32_t GetCurrentImageIndex() const;
    uint32_t GetMaxFrameCount() const;



public:
	Swapchain()                             = default;
	Swapchain(const Swapchain&)             = delete;
	Swapchain(const Swapchain&&)            = delete;
	Swapchain& operator=(const Swapchain&)  = delete;
	Swapchain& operator=(const Swapchain&&) = delete;
	~Swapchain();

private:
    void CreateResources();
    void DestroyResources();

private:
	Native::Swapchain m_Swapchain;
    std::vector<std::vector<vk::ImageView>> m_ImageView;
    bool m_Initalized = false;

	DepthStencil m_DepthStencil;
    uint32_t m_CurrentImageIndex;

    Native::PhysicalDevice* m_PhysicalDevice;
	vk::Device m_LogicalDevice;
	vk::SurfaceKHR m_Surface;
};
} // namespace VT::Vulkan
