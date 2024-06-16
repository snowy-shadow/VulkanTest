module;
#include <memory>
#include <functional>
#include "EngineMacro.h"

// compiler bug, hack fix. remove when fixed
#include <format>

module Application;

import VulkanTest;

import ImageLayer;

// namespace VT
// {
// template <>
// // requires requires { std::is_base_of<Event, Type::GetType()>; }
// bool Dispatch<WindowCloseEvent>(Event& E, std::function<bool(WindowCloseEvent&)> Function)
// {
//     // check Event and the handling function match
//     E.m_Handled = Function(dynamic_cast<WindowCloseEvent&>(E));
//     return E.m_Handled;
// }
// } // namespace VT

Application::Application() : m_Window(std::unique_ptr<VT::Window>(VT::Window::Create()))
{
    m_Window->SetEventCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    m_LayerStack.PushLayer(new ImageLayer());
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
    if (VT::Dispatch<VT::WindowCloseEvent>(E, std::bind(&Application::OnWindowClose, this, std::placeholders::_1)))
    {
        return;
    }

    for (auto Layer : m_LayerStack)
    {
        Layer->OnEvent(E);
        if (E.Handled())
        {
            break;
        }
    }

    VT_TRACE("{}", E);
}

bool Application::OnWindowClose(VT::WindowCloseEvent&)
{
    m_Running = false;
    return true;
}
