#include "Window.h"

VT::Window::Window(const std::array<int, 2>& Dimension_XY, const char* WindowName)
{
	if (!glfwInit()){throw std::runtime_error("Unable to initalize GLFW");}

	if (!glfwVulkanSupported()){throw std::runtime_error("GLFW did not find Vulkan loader or LCD");}


	// do not create opengl context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Window = glfwCreateWindow(Dimension_XY.at(0), Dimension_XY.at(1), WindowName, nullptr, nullptr);

	// associate window obj with current instance
	glfwSetWindowUserPointer(m_Window, this);
}

VT::Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}
