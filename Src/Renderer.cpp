#include "Renderer.h"


namespace VT
{

	void Renderer::createSwapChain(const uint32_t& Amount)
	{
		int Width, Height;
		glfwGetFramebufferSize(m_Window->m_Window, &Width, &Height);

		for(uint32_t i = 0; i < Amount; i++)
		{
			m_SwapChain.emplace_back(std::array{ static_cast<uint32_t>(Width), static_cast<uint32_t>(Height) });
		}

		for(auto& SC : m_SwapChain)
		{
			SC.bindDevices(m_Instance->m_LogicalDevice, m_Instance->m_PhysicalDevice, m_Instance->m_Surface);

			SC.createSwapChain(
				{ {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear} },
				{ vk::PresentModeKHR::eFifo }, 
				{ vk::CompositeAlphaFlagBitsKHR::eOpaque },
				{ vk::SurfaceTransformFlagBitsKHR::eIdentity }
			);
		}
	}

	void Renderer::bindInstance(Instance& Instance){ m_Instance = &Instance; }

	void Renderer::bindWindow(Window& Window) { m_Window = &Window; }


	void Renderer::update()
	{

	}
	void Renderer::destroy()
	{
		for (auto& SC : m_SwapChain) { SC.destroySwapChain(); }
	}
}

