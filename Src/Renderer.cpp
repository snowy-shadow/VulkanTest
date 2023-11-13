#include "Renderer.h"


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

	void Renderer::addShader(const std::vector<DXC_ShaderFileInfo>& Files)
	{
		for (const auto& F : Files)
		{
			m_GraphicsPipeLine.loadFile(F);
		}
		m_GraphicsPipeLine.compileFiles("../spv");
	}

	void Renderer::update()
	{

	}
	void Renderer::destroy()
	{
		for (auto& SC : m_SwapChain) { SC.destroySwapChain(); }
	}

	/*
	 * ==================================================
	 *					    PRIVATE
	 * ==================================================
	 */
}

