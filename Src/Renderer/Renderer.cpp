#include "Renderer.h"
#include "SwapChain.h"

#include <iostream>
#include <ranges>

namespace VT
{
	void Renderer::createSwapChain(std::string SwapchainName, VT::Swapchain Swapchain)
	{
		// insert swapchain
		auto [SwapchainPtr, Result] = m_SwapChains.try_emplace(std::move(SwapchainName), std::move(Swapchain));
	
		// alias, less typing
		auto& SC = SwapchainPtr->second;
	
		if (!Result) { throw std::runtime_error("Swapchain Name conflict! Attempt to create swapchain with name " + SwapchainPtr->first); }

		// bind device
		SC.bindDevices(m_PhysicalDevice, m_LogicalDevice, m_Surface);
		
		SC.createSwapChain();
	}

	void Renderer::bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices)
	{
		destroy();
		std::tie(m_LogicalDevice, m_PhysicalDevice, m_Surface) = Devices;
	}

	void Renderer::bindWindow(Window& Window) { m_Window = &Window; }

	void Renderer::createGraphicsPipeline
	(
		std::string Name,
		const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles,
		vk::GraphicsPipelineCreateInfo PipelineInfo
	)
    {
		assert(m_LogicalDevice && !m_Pipelines.contains(Name));

		// compile shaders
		auto ShaderSpvs{ m_ShaderCompiler.compileShaders(ShaderFiles)};

		// Graphics pipline struct
		std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfos;
	
		// load all shader spv
		for(std::size_t i = 0; i < ShaderSpvs.size(); i++)
		{
			ShaderStageInfos.push_back
			({
				.stage = ShaderFiles[i].Stage,
				.module = m_LogicalDevice.createShaderModule
				(
					{
						.codeSize = ShaderSpvs[i].size(),
						.pCode = reinterpret_cast<uint32_t*>(ShaderSpvs[i].data())
					}
				),

				.pName = "main"
			});
		}
	

		PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStageInfos.size());
		PipelineInfo.pStages = ShaderStageInfos.data();

		auto [Result, Pipeline] = m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo);

		if (Result != vk::Result::eSuccess) { throw std::runtime_error("Failed to create Graphics Pipeline : " + Name); }

		m_Pipelines.emplace(std::move(Name), Pipeline);

		// delete shader modules
		for(auto& SM : ShaderStageInfos)
		{
			std::cout << "deleting : " << SM.module << "\n";
			m_LogicalDevice.destroyShaderModule(SM.module);
		}
    }

	void Renderer::update()
	{

	}
	Renderer::~Renderer()
	{
		destroy();
	}




	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */

	void Renderer::destroy() noexcept
	{
		// ORDER MATTERS
		// Pipeline
		for (auto& P : m_Pipelines | std::views::values) { m_LogicalDevice.destroyPipeline(P); }

		// Swapchain
		for (auto& SC : m_SwapChains | std::views::values) { SC.destroySwapChain(); }
	}
}

