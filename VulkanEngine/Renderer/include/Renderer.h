#pragma once

#include "Swapchain.h"
/*
 * <atlcomcli.h> must be included after all instances of <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/
#include "Window.h"
#include "PhysicalDevice.h"
#include "ShaderCompiler.h"
#include "DependencyGraph.h"
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
		void createMainSwapChain();
		std::vector<vk::CommandBuffer>& createCommandBuffer();
		void createImage();
		void recordRenderPass(const vk::CommandBuffer& CB, uint32_t Frame);

		// General Helper
		void createSwapChain(bool GraphicsPresent, vk::SwapchainCreateInfoKHR SwapchainCreateInfo, Swapchain::Capabilities Queries);
		bool createPipelineLayout(std::string Name, const vk::PipelineLayoutCreateInfo& LayoutInfo);
		bool createRenderPass(std::string Name, const vk::RenderPassCreateInfo& RenderPassInfo);

		/*
		 * Creates graphics pipeline
		 * Name - Name of pipeline
		 * ShaderFiles - files to compile to shaders
		 * LayoutInfo - PipelineLayoutInfo to create layout from
		 * RenderPassInfo - RenderPassCreateInfo to create renderpass from
		 * PipelineInfo - GraphicsPipeline infos, shader stages will be compiled and inserted based on ShaderFiles
		*/
		bool createGraphicsPipeline(
			std::span<const File::DXC_ShaderFileInfo> ShaderFiles,
			const std::string& GraphicsPipelineName,
			vk::GraphicsPipelineCreateInfo PipelineInfo,
			const std::string& PipelineLayoutName,
			const std::string& RenderPassName);

		void destroy() noexcept;

		std::vector<ImageGroup> m_Images;
		vk::Semaphore ImageAvailable, RenderFinished;
		vk::Fence Fence;

		// SwapChain
		DependencyGraph<
			vk::Pipeline, vk::PipelineLayout, vk::RenderPass,
			vk::CommandPool, std::vector<vk::CommandBuffer>, vk::Queue,
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
