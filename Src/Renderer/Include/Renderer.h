#pragma once

#include "SwapChain.h"
/*
 * Must be included after all instances, of #include <vulkan/vulkan_hash.hpp>
 * <atlcomcli.h> breaks <vulkan/vulkan_hash.hpp>, they must come after
*/

#include "Window.h"
#include "Instance.h"
#include "GraphicPipelineConfig.h"
#include "ShaderCompiler.h"

#include <list>
#include <unordered_set>

namespace VT
{
	class Renderer
	{
	public:
		explicit Renderer(Instance& Instance, Window& Window);

		void createSwapChain(const std::unordered_set<std::string>& SwapChainName);
		void bindInstance(Instance& Instance);
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

		std::string m_CurrentLogicDevice;
		// Instance handle
		Instance* m_Instance{ nullptr };
		// Window handle
		Window* m_Window{ nullptr };
	};
}
