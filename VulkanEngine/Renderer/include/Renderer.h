#pragma once

#include "Swapchain.h"
/*
 * <atlcomcli.h> must be included after all instances of <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/
#include "Window.h"
#include "PhysicalDevice.h"
#include "DependencyGraph.h"
#include "Buffer.h"
#include "ImageGroup.h"

namespace VT
{
	class Renderer
	{
	public:
		explicit Renderer(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR>, Window*);

		void bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices, Window* W);

		void init();
		
		void update();

		Renderer() = default;
		~Renderer();
		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;

	
	private:
		void createMainGraphicsPipeline();
		void recreateSwapchain(std::string Name);
		void createMainSwapChain();
		std::vector<vk::CommandBuffer>& createCommandBuffer();
		void createImage();
		void createVertexBuffer();
		void recordRenderPass(const vk::CommandBuffer& CB, uint32_t Frame);
		void createDescriptorSet();
		void updateMVP(Buffer& Buff);

		// General Helper
		void resetSemaphore(const std::vector<std::reference_wrapper<vk::Semaphore>>& Semaphores);
		void createSwapChain(bool GraphicsPresent, vk::SwapchainCreateInfoKHR SwapchainCreateInfo, Swapchain::Capabilities Queries);
		bool createRenderPass(std::string Name, const vk::RenderPassCreateInfo& RenderPassInfo);


		void destroy() noexcept;

		std::vector<ImageGroup> m_Images;
		vk::Semaphore ImageAvailable, RenderFinished;
		vk::Fence Fence;

		// SwapChain
		DependencyGraph<
			std::vector<vk::PipelineShaderStageCreateInfo>,
			vk::RenderPass,
			vk::CommandPool, std::vector<vk::CommandBuffer>, vk::Queue,
			Buffer, std::vector<std::array<float, 5>>,
			vk::DescriptorSetLayout, vk::DescriptorPool, std::vector<vk::DescriptorSet>,
			Swapchain, vk::Framebuffer>
		m_DependencyGraph;

		uint32_t m_MaxFrameCount{ 2 };
		uint32_t m_CurrentFrame{ 0 };

		// Handles
		Window* m_Window{nullptr};
		vk::Device m_LogicalDevice;
		PhysicalDevice const* m_PhysicalDevice {nullptr};
		vk::SurfaceKHR m_Surface;
	};
}
