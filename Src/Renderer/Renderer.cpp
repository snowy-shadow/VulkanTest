#include "Renderer.h"

#include <iostream>


namespace VT
{

	void Renderer::createSwapChain(const std::unordered_set<std::string_view>& SwapChainName)
	{
		int Width, Height;
		glfwGetFramebufferSize(m_Window->m_Window, &Width, &Height);

		auto Name = SwapChainName.cbegin();

		for(uint32_t i = 0; i < SwapChainName.size(); i++)
		{
			m_SwapChain.emplace_back(std::array{ static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) }, 2);

			const auto SC = std::prev(m_SwapChain.end());
			SC->bindDevice(m_Instance->m_PhysicalDevice, m_Instance->m_LogicalDevice, m_Instance->m_Surface);

			SC->setProperties
			(
				{ {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear} },
				{ vk::PresentModeKHR::eFifo },
				{ vk::CompositeAlphaFlagBitsKHR::eOpaque },
				{ vk::SurfaceTransformFlagBitsKHR::eIdentity }
			);
			SC->createSwapChain();

			m_SwapChainReference.emplace(*Name, i);
			Name = std::next(Name);
		}
	}

	void Renderer::bindInstance(Instance& Instance){ m_Instance = &Instance; }

	void Renderer::bindWindow(Window& Window) { m_Window = &Window; }

	/*void Renderer::addShaderStage(const std::vector<DXC_ShaderFileInfo>& Files)
	{
		for (const auto& F : Files)
		{
			m_GraphicsPipeLine.appendShaderStage(
				{
					.stage = vk::ShaderStageFlagBits::eFragment,
					.module = m_GraphicsPipeLine.createShaderModule(F, m_Instance->m_LogicalDevice),
					.pName = "main"
				}
			);
		}
	}*/

	void Renderer::createGraphicsPipeline(std::string Name, const std::vector<File::DXC_ShaderFileInfo>& ShaderFiles, GraphicPipeline& PipelineInfo)
    {
		// compile shaders
		auto ShaderSpvs{ m_ShaderCompiler.compileShaders(ShaderFiles)};

		// Graphics pipline struct
		std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageInfos;

		// load all shader spv
		for(int i = 0; i < ShaderSpvs.size(); i++)
		{
			std::cout << "Begin\n";
			for(auto& a : ShaderSpvs[i])
			{
				std::cout << a;
			}

			std::cout << "\n\n\n";
			ShaderStageInfos.push_back
			({
				.stage = ShaderFiles[i].Stage,
				.module = m_Instance->m_LogicalDevice.createShaderModule
				({
					.codeSize = ShaderSpvs[i].size(),
					.pCode = reinterpret_cast<uint32_t*>(&ShaderSpvs[i])
				}),

				.pName = "main"
			});
		}

		auto [Result, Pipeline] = m_Instance->m_LogicalDevice.createGraphicsPipeline(nullptr, PipelineInfo.getGraphicPipelineInfo(ShaderStageInfos));

		if (Result != vk::Result::eSuccess) { throw std::runtime_error("Failed to create Graphics Pipeline : " + Name); }

		m_Pipelines.emplace(std::move(Name), Pipeline);

		// delete shader modules
		for(auto& SM : ShaderStageInfos)
		{
			m_Instance->m_LogicalDevice.destroyShaderModule(SM.module);
		}
    }

	void Renderer::update()
	{

	}
	void Renderer::destroy()
	{
		for(auto& P : m_Pipelines) { m_Instance->m_LogicalDevice.destroyPipeline(P.second); }
	
		for (auto& SC : m_SwapChain) { SC.destroySwapChain(); }
	}




	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */
}

