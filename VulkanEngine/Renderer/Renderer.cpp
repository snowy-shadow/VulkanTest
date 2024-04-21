#include "Renderer.h"

#include <ranges>

namespace VT
{
	void Renderer::createSwapChain(std::string SwapchainName, vk::SwapchainCreateInfoKHR SwapchainCreateInfo, Swapchain::Capabilities Queries)
	{
		// insert swapchain
		auto [SwapchainPtr, Result] = m_Swapchains.try_emplace(SwapchainName, Swapchain{});
	
		if (!Result) { throw std::runtime_error("Swapchain creation failed! Attempted to create swapchain with name " + std::move(SwapchainName)); }
	
		const auto& CB = SwapchainPtr->second.queryCapabilities(std::move(Queries), m_PhysicalDevice, m_Surface);

		SwapchainCreateInfo.minImageCount = CB.minImageCount;
		SwapchainCreateInfo.imageExtent = CB.imageExtent;
		SwapchainCreateInfo.imageFormat = CB.surfaceFormat[0].format;
		SwapchainCreateInfo.imageColorSpace = CB.surfaceFormat[0].colorSpace;
		SwapchainCreateInfo.imageUsage = CB.imageUsage[0];
		SwapchainCreateInfo.preTransform = CB.surfaceTransform[0];
		SwapchainCreateInfo.compositeAlpha = CB.compositeAlpha[0];
		SwapchainCreateInfo.presentMode = CB.presentMode[0];
		SwapchainCreateInfo.imageArrayLayers = CB.arrayLayers;
		SwapchainCreateInfo.surface = m_Surface;
		
		
		if(!m_PhysicalDevice->graphicsQueueCanPresent())
		{
			SwapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			SwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(m_PhysicalDevice->getGraphicsPresentQueueIndices().size());
			SwapchainCreateInfo.pQueueFamilyIndices = m_PhysicalDevice->getGraphicsPresentQueueIndices().data();
		}
		SwapchainPtr->second.createSwapchain(std::move(SwapchainCreateInfo), m_LogicalDevice);
	}

	vk::SwapchainCreateInfoKHR Renderer::getSwapchainInfo(std::string Name)
	{
		return m_Swapchains[Name].getSwapchainCreateInfo();
	}

	void Renderer::bindDevices(std::tuple<vk::Device, PhysicalDevice const*, vk::SurfaceKHR> Devices)
	{
		std::tie(m_LogicalDevice, m_PhysicalDevice, m_Surface) = Devices;
	}

	void Renderer::bindWindow(Window& Window) { m_Window = &Window; }

	void Renderer::createGraphicsPipeline
	(
		std::string Name,
		const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles,
		const vk::PipelineLayoutCreateInfo& LayoutInfo,
		const vk::RenderPassCreateInfo& RenderPassInfo, 
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

		// leak, wrap everything up into obj
		PipelineInfo.layout = m_LogicalDevice.createPipelineLayout(LayoutInfo);
		PipelineInfo.renderPass = m_LogicalDevice.createRenderPass(RenderPassInfo);

		auto [Result, Pipeline] = m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo);

		// delete shader modules
		for(const auto& SM : ShaderStageInfos) { m_LogicalDevice.destroyShaderModule(SM.module); }
	
		// check Pipeline result
		if (Result != vk::Result::eSuccess) { throw std::runtime_error("Failed to create Graphics Pipeline : " + Name); }

		m_Pipelines.emplace(std::move(Name), Pipeline);
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
		for (const auto& P : m_Pipelines | std::views::values) { m_LogicalDevice.destroyPipeline(P); }

		// Swapchain
		for (auto& SC : m_Swapchains | std::views::values) { SC.destroySwapchain(m_LogicalDevice); }
	}
}

