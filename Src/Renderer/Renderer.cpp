#include "Renderer.h"

#include <iostream>
#include <ranges>

namespace VT
{
	void Renderer::createSwapChain(const std::unordered_set<std::string>& SwapChainNames)
	{
		int Width, Height;
		glfwGetFramebufferSize(m_Window->m_Window, &Width, &Height);

		for (const auto& Name : SwapChainNames)
		{
			// insert swapchain
			auto [SC, Result] = m_SwapChains.try_emplace(Name, std::array{ static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) }, 2);

			if (!Result) { throw std::runtime_error("Swapchain Name conflict! Attempt to create swapchain with name " + Name); }

			SC->second.bindDevice(m_PhysicalDevice, m_CurrentLogicDevice->second, m_Surface);
			SC->second.setProperties
			(
				{ {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear} },
				{ vk::PresentModeKHR::eFifo },
				{ vk::CompositeAlphaFlagBitsKHR::eOpaque },
				{ vk::SurfaceTransformFlagBitsKHR::eIdentity }
			);
			SC->second.createSwapChain();
		}
	}

	void Renderer::bindDevices(std::tuple<std::unordered_map<std::string, vk::Device> const*, PhysicalDevice const*, vk::SurfaceKHR> Devices)
	{
		std::tie(m_LogicalDevices, m_PhysicalDevice, m_Surface) = Devices;
	}

	void Renderer::bindWindow(Window& Window) { m_Window = &Window; }

	void Renderer::createGraphicsPipeline(std::string Name, const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles, const GraphicPipelineConfig& PipelineInfo)
    {
		assert(!m_Pipelines.contains(Name));

		// compile shaders
		auto ShaderSpvs{ m_ShaderCompiler.compileShaders(ShaderFiles)};

		// Graphics pipline struct
		std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfos;

		// Current Logical Device
		const vk::Device LogicalDevice = m_CurrentLogicDevice->second;

		// load all shader spv
		for(std::size_t i = 0; i < ShaderSpvs.size(); i++)
		{
			ShaderStageInfos.push_back
			({
				.stage = ShaderFiles[i].Stage,
				.module = LogicalDevice.createShaderModule
				(
					{
						.codeSize = ShaderSpvs[i].size(),
						.pCode = reinterpret_cast<uint32_t*>(ShaderSpvs[i].data())
					}
				),

				.pName = "main"
			});
		}

		auto [Result, Pipeline] = LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo.getGraphicPipelineCreateInfo(ShaderStageInfos));

		if (Result != vk::Result::eSuccess) { throw std::runtime_error("Failed to create Graphics Pipeline : " + Name); }

		m_Pipelines.emplace(std::move(Name), Pipeline);

		// delete shader modules
		for(auto& SM : ShaderStageInfos)
		{
			std::cout << "deleting : " << SM.module << "\n";
			LogicalDevice.destroyShaderModule(SM.module);
		}
    }

	void Renderer::selectLogicalDevice(std::string Name)
	{
		auto Iter = m_LogicalDevices->find(Name);
		if(Iter == m_LogicalDevices->end()) { throw std::runtime_error("Logical Device : " + std::move(Name) + " does not exist"); }
		m_CurrentLogicDevice = Iter;
	}

	void Renderer::update()
	{

	}
	Renderer::~Renderer()
	{
		// ORDER MATTERS
		// Pipeline
		for (auto& P : m_Pipelines) { m_CurrentLogicDevice->second.destroyPipeline(P.second); }

		// Swapchain
		for (auto& SC : m_SwapChains | std::views::values) { SC.destroySwapChain(); }
	}




	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */
}

