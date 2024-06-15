module;
#include <GLFW/glfw3.h>
#include "EngineMacro.h"

// error C3688: invalid literal suffix 'sv'. compiler bug, remove include when fixed
#include <format>
module Platform.Windows.Window;

import Event;
import Log;

namespace VT
{
static bool s_GLFW_Initialized = false;
Window* Window::Create(const WindowProperties& Properties) { return new Windows::Window(Properties); }

} // namespace VT

namespace VT::Windows
{
static void GLFWErrorCallBack(int Error, const char* Description)
{
    VT_CORE_ERROR("GLFW Error ({0}) : {1}", Error, Description);
}

Window::Window(const WindowProperties& Properties) : m_Data(Properties, true, {})
{
    if (!VT::s_GLFW_Initialized)
    {
        int Result = glfwInit();

        VT_CORE_ASSERT(Result, "Failed to init GLFW")

        glfwSetErrorCallback(GLFWErrorCallBack);

        VT::s_GLFW_Initialized = Result == GLFW_TRUE;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(
        static_cast<int>(m_Data.Width), static_cast<int>(m_Data.Height), m_Data.Title, nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, &m_Data);

    /* =================================
     *           Set Event Callback
     *  =================================
     */

    glfwSetWindowSizeCallback(m_Window,
                              [](GLFWwindow* Window, int Width, int Height)
    {
        WindowInfo& Data = *reinterpret_cast<WindowInfo*>(glfwGetWindowUserPointer(Window));
        Data.Width       = Width;
        Data.Height      = Height;

        VT::WindowResizeEvent E {{{static_cast<unsigned int>(Width), static_cast<unsigned int>(Height)}}};
        Data.EventCallBack(E);
    });

    glfwSetWindowCloseCallback(m_Window,
                               [](GLFWwindow* Window)
    {
        WindowInfo& Data = *reinterpret_cast<WindowInfo*>(glfwGetWindowUserPointer(Window));
        VT::WindowCloseEvent E;
        Data.EventCallBack(E);
    });

    glfwSetKeyCallback(m_Window,
                       [](GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
    {
        WindowInfo& Data = *reinterpret_cast<WindowInfo*>(glfwGetWindowUserPointer(Window));

        // Warn unused
        (void) ScanCode;
        (void) Mods;

        switch (Action)
        {
            case GLFW_PRESS:
            {
                VT::KeyPressEvent E {static_cast<unsigned int>(Key), 0};
                Data.EventCallBack(E);
                break;
            }

            case GLFW_REPEAT:
            {
                VT::KeyPressEvent E {static_cast<unsigned int>(Key), 1};
                Data.EventCallBack(E);
                break;
            }
            case GLFW_RELEASE:
            {
                VT::KeyReleaseEvent E {static_cast<unsigned int>(Key)};
                Data.EventCallBack(E);
                break;
            }
        }
    });

    glfwSetMouseButtonCallback(m_Window,
                               [](GLFWwindow* Window, int Button, int Action, int Mods)
    {
        WindowInfo& Data = *reinterpret_cast<WindowInfo*>(glfwGetWindowUserPointer(Window));

        // warn unused
        (void) Mods;

        switch (Action)
        {
            case GLFW_PRESS:
            {
                VT::MouseButtonPressEvent E {static_cast<unsigned int>(Button)};
                Data.EventCallBack(E);
                break;
            }

            case GLFW_RELEASE:
            {
                VT::MouseButtonReleaseEvent E {static_cast<unsigned int>(Button)};
                Data.EventCallBack(E);
                break;
            }
        }
    });

    glfwSetScrollCallback(m_Window,
                          [](GLFWwindow* Window, double XOffset, double YOffset)
    {
        WindowInfo& Data = *reinterpret_cast<WindowInfo*>(glfwGetWindowUserPointer(Window));

        VT::MouseScrollEvent E {{{static_cast<float>(XOffset), static_cast<float>(YOffset)}}};
        Data.EventCallBack(E);
    });

    glfwSetCursorPosCallback(m_Window,
                             [](GLFWwindow* Window, double XPos, double YPos)
    {
        WindowInfo& Data = *reinterpret_cast<WindowInfo*>(glfwGetWindowUserPointer(Window));
        VT::MouseMoveEvent E {{{static_cast<float>(XPos), static_cast<float>(YPos)}}};
        Data.EventCallBack(E);
    });
}

void Window::OnUpdate()
{
    glfwPollEvents();
    // VT_CORE_INFO("Window Update");
}

unsigned int Window::GetWidth() const { return m_Data.Width; }
unsigned int Window::GetHeight() const { return m_Data.Height; }

void Window::SetVSync(bool Enable) { m_Data.VSync = Enable; }
bool Window::IsVSync() const { return m_Data.VSync; }

Window::~Window() { glfwDestroyWindow(m_Window); }

} // namespace VT::Windows
