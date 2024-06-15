module;
#include <memory>
#include <functional>
#include "EngineMacro.h"

// compiler bug, hack fix. remove when fixed
#include <format>

module Application;

import VulkanTest;

Application::Application() : m_Window(std::unique_ptr<VT::Window>(VT::Window::Create()))
{
    m_Window->SetEventCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

void Application::Run()
{
    while (m_Running)
    {
        m_Window->OnUpdate();
    }
}

void Application::OnEvent(VT::Event& E)
{
    VT::EventDispatcher Dispatcher {E};
    Dispatcher.Dispatch<VT::WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

    VT_TRACE("{}", E);
}

bool Application::OnWindowClose(VT::WindowCloseEvent&)
{
    m_Running = false;
    return true;
}
