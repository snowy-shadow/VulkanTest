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

	void Renderer::update()
	{
		// maybe put this into VT::Window
		while (!glfwWindowShouldClose(m_Window.m_Window))
		{
			glfwPollEvents();
		}
	}
}

