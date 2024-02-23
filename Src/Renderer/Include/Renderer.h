#pragma once

#include "Swapchain.h"
/*
 * Must be included after all instances, of #include <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/

#include "Window.h"
#include "PhysicalDevice.h"
#include "ShaderCompiler.h"

#include <unordered_map>

namespace VT
{
	class Renderer
	{
	public:
		void createSwapChain(std::string SwapchainName, VT::Swapchain Swapchain);

		void bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices);
		void bindWindow(Window& Window);

		vk::SwapchainCreateInfoKHR getSwapchainInfo(std::string Name) const;

		/*
		 * Creates graphics pipeline
		 * Name - Name of pipeline
		 * ShaderFiles - files to compile to shaders
		 * RenderPassInfo - RenderPassCreateInfo to create renderpass from
		 * PipelineInfo - GraphicsPipeline infos, shader stages will be compiled and inserted based on ShaderFiles
		*/
		void createGraphicsPipeline(std::string Name, const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles, vk::RenderPassCreateInfo RenderPassInfo, vk::GraphicsPipelineCreateInfo PipelineInfo);

		void update();

		Renderer() = default;
		~Renderer();
		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;

	
	private:
		bool m_Constructed{false};
		void destroy() noexcept;
	
		ShaderCompiler m_ShaderCompiler;
	
		// Device handles
		std::unordered_map<std::string, Swapchain> m_Swapchains;
		std::unordered_map<std::string, vk::Pipeline> m_Pipelines;
		
		// Handles
		vk::Device m_LogicalDevice;
		PhysicalDevice const* m_PhysicalDevice {nullptr};
		vk::SurfaceKHR m_Surface;

		// Window handle
		Window* m_Window{ nullptr };
	};
}
