#pragma once

#include "SwapChain.h"
/*
 * Must be included after all instances, of #include <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/

#include "Window.h"
#include "PhysicalDevice.h"
#include "GraphicPipelineConfig.h"
#include "ShaderCompiler.h"

#include <unordered_set>
#include <unordered_map>

namespace VT
{
	class Renderer
	{
	public:
		void createSwapChain(const std::unordered_set<std::string>& SwapChainName);

		void bindDevices(std::tuple<std::unordered_map<std::string, vk::Device> const*, PhysicalDevice const*, vk::SurfaceKHR> Devices);
		void bindWindow(Window& Window);

//		void addShaderStage(const std::vector<DXC_ShaderFileInfo>&);

		void createGraphicsPipeline(std::string Name, const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles, const GraphicPipelineConfig& PipelineInfo);

		void selectLogicalDevice(std::string Name);
		void update();

		Renderer() = default;
		~Renderer();
		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;

	
	private:
		ShaderCompiler m_ShaderCompiler;

		std::unordered_map<std::string, SwapChain> m_SwapChains;

		std::unordered_map<std::string, vk::Pipeline> m_Pipelines;

		// Device handles
		std::unordered_map<std::string, vk::Device> const* m_LogicalDevices {nullptr};
		std::unordered_map<std::string, vk::Device>::const_iterator m_CurrentLogicDevice{};

		PhysicalDevice const* m_PhysicalDevice {nullptr};
		vk::SurfaceKHR m_Surface;

		// Window handle
		Window* m_Window{ nullptr };
	};
}
