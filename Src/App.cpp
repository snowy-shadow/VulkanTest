module;
#include <memory>
#include <functional>
#include "EngineMacro.h"

module Application;

import VulkanTest;

import ImageLayer;

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
