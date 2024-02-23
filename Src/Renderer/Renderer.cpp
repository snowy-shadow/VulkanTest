#include "Renderer.h"

#include <ranges>

namespace VT
{
	void Renderer::createSwapChain(std::string SwapchainName, VT::Swapchain Swapchain)
	{
		// insert swapchain
		auto [SwapchainPtr, Result] = m_Swapchains.try_emplace(SwapchainName, std::move(Swapchain));
	
		if (!Result) { throw std::runtime_error("Swapchain creation failed! Attempted to create swapchain with name " + std::move(SwapchainName)); }
	
		auto& SC = SwapchainPtr->second;
	
		SC.queryCapabilities(m_PhysicalDevice, m_Surface);
	
		SC.m_SwapchainInfo.minImageCount = SC.m_SwapchainRequest.minImageCount;
		SC.m_SwapchainInfo.imageExtent = SC.m_SwapchainRequest.imageExtent;
		SC.m_SwapchainInfo.imageFormat = SC.m_SwapchainRequest.surfaceFormat[0].format;
		SC.m_SwapchainInfo.imageColorSpace = SC.m_SwapchainRequest.surfaceFormat[0].colorSpace;
		SC.m_SwapchainInfo.imageUsage = SC.m_SwapchainRequest.imageUsage[0];
		SC.m_SwapchainInfo.preTransform = SC.m_SwapchainRequest.surfaceTransform[0];
		SC.m_SwapchainInfo.compositeAlpha = SC.m_SwapchainRequest.compositeAlpha[0];
		SC.m_SwapchainInfo.presentMode = SC.m_SwapchainRequest.presentMode[0];
		SC.m_SwapchainInfo.imageArrayLayers = SC.m_SwapchainRequest.arrayLayers;
		SC.m_SwapchainInfo.surface = m_Surface;
		
		
		if(!m_PhysicalDevice->graphicsQueueCanPresent())
		{
			SC.m_SwapchainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			SC.m_SwapchainInfo.queueFamilyIndexCount = static_cast<uint32_t>(m_PhysicalDevice->getGraphicsPresentQueueIndices().size());
			SC.m_SwapchainInfo.pQueueFamilyIndices = m_PhysicalDevice->getGraphicsPresentQueueIndices().data();
		}
		SwapchainPtr->second.createSwapchain(m_LogicalDevice);
	}

	vk::SwapchainCreateInfoKHR Renderer::getSwapchainInfo(std::string Name) const
	{
		return m_Swapchains.at(std::move(Name)).m_SwapchainInfo;
	}

	void Renderer::bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices)
	{
		if(m_Constructed) { destroy(); }
		std::tie(m_LogicalDevice, m_PhysicalDevice, m_Surface) = Devices;
	}

	void Renderer::bindWindow(Window& Window) { m_Window = &Window; }

	void Renderer::createGraphicsPipeline
	(
		std::string Name,
		const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles,
		vk::RenderPassCreateInfo RenderPassInfo, 
		vk::GraphicsPipelineCreateInfo PipelineInfo
	)
    {
		// cannot have name collision
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
		PipelineInfo.renderPass = m_LogicalDevice.createRenderPass(std::move(RenderPassInfo));

		auto [Result, Pipeline] = m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo);

		// delete shader modules
		for(auto& SM : ShaderStageInfos) { m_LogicalDevice.destroyShaderModule(SM.module);	}
	
		// check Pipeline result
		if (Result != vk::Result::eSuccess) { throw std::runtime_error("Failed to create Graphics Pipeline : " + Name); }

		m_Pipelines.emplace(std::move(Name), Pipeline);
    }

	void Renderer::update()
	{

	}
	Renderer::~Renderer()
	{
		if(m_Constructed) { destroy(); }
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
		for (auto& SC : m_Swapchains | std::views::values) { SC.destroySwapchain(m_LogicalDevice); }
	}
}

