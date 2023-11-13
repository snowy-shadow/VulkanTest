#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <stdexcept>


namespace VT
{
	struct Window
	{
		Window(const std::array<int, 2>& Dimension_XY, const char* WindowName);
		~Window();


		Window(const Window&) = delete;
		Window& operator = (const Window&) = delete;

		GLFWwindow* m_Window{ nullptr };
	};
}