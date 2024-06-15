module;
#include <GLFW/glfw3.h>
#include "EngineMacro.h"

// error C3688: invalid literal suffix 'sv'. compiler bug, remove include when fixed
#include <format>
module Platform.Windows.Window;

import Log;

namespace VT
{
static bool s_GLFW_Initialized = false;
Window* Window::Create(const WindowProperties& Properties) { return new Windows::Window(Properties); }
} // namespace VT

namespace VT::Windows
{
Window::Window(const WindowProperties& Properties) : m_Data(Properties, true, {})
{
    if (!VT::s_GLFW_Initialized)
    {
        int Result = glfwInit();

        VT_CORE_ASSERT(Result, "Failed to init GLFW")

        VT::s_GLFW_Initialized = Result == GLFW_TRUE;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(
        static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height), m_Data.Title, nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, &m_Data);
}

void Window::OnUpdate()
{
    glfwPollEvents();
    VT_CORE_INFO("Window Update");
}

unsigned int Window::GetWidth() const { return m_Data.Width; }
unsigned int Window::GetHeight() const { return m_Data.Height; }

void Window::SetVSync(bool Enable) { m_Data.VSync = Enable; }
bool Window::IsVSync() const { return m_Data.VSync; }

Window::~Window() { glfwDestroyWindow(m_Window); }

} // namespace VT::Windows
