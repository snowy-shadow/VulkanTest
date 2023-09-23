#include "Renderer.h"

namespace VT
{
	Renderer::Renderer()
	{


	}

	Renderer::~Renderer()
	{
		
	}

	GLFWwindow* Renderer::getWindow()
	{
		return m_Window.m_Window;
	}

	void Renderer::createSwapChain(const PhysicalDevice& PD, const uint32_t& Amount)
	{
		for(uint32_t i = 0; i < Amount; i++)
		{
			m_SwapChain.emplace_back(SwapChain());
		}

		for(auto& SC : m_SwapChain)
		{
			SC.createSwapChain(PD);
		}
	}


	void Renderer::update()
	{

	}
}

