module;
#include <functional>
#include "EngineMacro.h"

#include <chrono>

module Application;

import ImageLayer;

Application::Application() : m_Window(std::unique_ptr<VT::Window>(VT::Window::Create(VT::WindowAPI::eGLFWwindow)))
{
    m_Input.reset(VT::Input::Create(*m_Window));
    m_Window->SetEventCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    m_LayerStack.PushLayer(new ImageLayer());

    m_Renderer.reset(new VT::Renderer(VT::RendererType::API::eVulkan, m_Window));
}

void Application::Run()
{
    while (m_Running)
    {
        //  auto [MouseX, MouseY] = m_Input->GetMouseXY();
        const VT::Timestep TimeDelta = VT::Timestep::abs(m_TimePoint.Tick(VT::Timepoint::Now()));
        m_Renderer->OnUpdate(TimeDelta);
        m_Renderer->BeginScene();
        m_Renderer->EndScene();
        m_Renderer->Submit();

        m_Window->OnUpdate();
        // VT_TRACE("{0}, {1}", MouseX, MouseY);
    }
}

void Application::OnEvent(VT::Event& E)
{
    if (VT::Dispatch<VT::WindowCloseEvent>(E, std::bind(&Application::OnWindowClose, this, std::placeholders::_1)))
    {
        return;
    }

    m_Renderer->OnEvent(E);

    for (auto Layer : m_LayerStack)
    {
        Layer->OnEvent(E);
        if (E.Handled())
        {
            break;
        }
    }

    // VT_TRACE("{}", E);
}

bool Application::OnWindowClose(VT::WindowCloseEvent&)
{
    m_Running = false;
    return true;
}
