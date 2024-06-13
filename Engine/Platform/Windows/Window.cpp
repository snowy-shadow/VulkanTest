#include "EngineMacro.h"
#include <GLFW/glfw3.h>
module Platform.Windows.Window;

namespace VT::Windows
{
static bool Window::m_GLFW_Initialized = false;

Window::Window(const WindowProperties& Properties) : m_Data({Properties, true, {}})
{
    if (!m_GLFW_Initialized)
    {
        int Result = glfwInit();

        VT_CORE_ASSERT(Result, "GLFW Init failed")

        m_GLFW_Initialized = Result == GLFW_TRUE;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, &m_Data);
}

void Window::OnUpdate() { glfwPollEvents(); }

unsigned int Window::GetWidth() const { return m_Data.Width; }
unsigned int Window::GetHeight() const { return m_Data.Height; }

void Window::SetVSync(bool Enable) { m_Data.VSync = Enable; }
void Window::IsVSync(bool Enable) { return m_Data.VSync; }

Window::~Window() { glfwDestroyWindow(m_Window); }

} // namespace VT::Windows
