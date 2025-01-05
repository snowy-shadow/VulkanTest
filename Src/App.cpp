module;
#include <functional>
#include "EngineMacro.h"
#include "glm/glm.hpp"

#include <vulkan/vulkan_structs.hpp>

module Application;

import ImageLayer;

Application::Application() : m_Window(std::unique_ptr<VT::Window>(VT::Window::Create(VT::WindowAPI::eGLFWwindow)))
{
    m_Input.reset(VT::Input::Create(*m_Window));
    m_Window->SetEventCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    m_LayerStack.PushLayer(new ImageLayer());

    m_Renderer.reset(new VT::Renderer(VT::GraphicsAPI::eVulkan, m_Window));

    CreateTrianglePipeline();

    m_Camera.reset(new VT::ProjectionCamera(m_Window->GetWidth(), m_Window->GetHeight()));
    m_CameraController.reset(new VT::CameraController);
    m_CameraController->BindInput(m_Input.get());

    m_Renderer->UploadView({.ProjectionMatrix = m_Camera->GetProjection(), .ViewMatrix = m_Camera->GetView()});

    m_Textures[0] = m_Renderer->CreateTexture({.File = "D:/ktz/Images/ramen.jpg"});
    m_Textures[1] = m_Renderer->CreateTexture({.File = "D:/ktz/Images/ramen2.jpg"});
    m_Textures[2] = m_Renderer->CreateTexture({.File = "should fail"});
}

void Application::CreateTrianglePipeline()
{
    auto& Manager = m_Renderer->PipelineManager();
    VT::DescriptorLayoutBindingInfo one
    {
        .Binding = 0,
        .DescriptorType = VT::DescriptorType::eUniformBuffer,
        .DescriptorCount = 1,
        .Stage = VT::ShaderStageFlagBit::eVertex,
    };

    Manager.CreateDescriptorLayout({one}
    , "Triangle");
    Manager.CreatePipelineLayout({
    });
    Manager.CreateGraphicsPipeline({
        .PipelineLayout = "TriangleLayout",
        .RenderPass = "TriangleRenderPass"
    }, "TrianglePipeline");
}

void Application::Run()
{
    while (m_Running)
    {
        //  auto [MouseX, MouseY] = m_Input->GetMouseXY();
        const VT::Timestep TimeDelta = VT::Timestep::abs(m_TimePoint.Tick(VT::Timepoint::Now()));
        OnUpdate(TimeDelta);

        m_Renderer->BeginScene();

        VT::GeometryRenderData Data {.ID = 0, .Model = {1.f}, .pTexture = &m_Textures[m_CurrentTexture]};

        if (TextureSwitch)
        {
            Data.ID = 2;
            TextureSwitch = false;
        }
        m_Renderer->UploadGeometry(Data);

        m_Renderer->EndScene();
        m_Renderer->Submit();

        m_Window->OnUpdate();
        // VT_TRACE("{0}, {1}", MouseX, MouseY);
    }
}

void Application::OnUpdate(const VT::Timestep& Ts)
{
    m_Renderer->OnUpdate(Ts);

    if (m_CameraController->OnUpdate(Ts))
    {
        m_Camera->SetTranslation(m_CameraController->GetTranslation());
        m_Camera->SetRotation(m_CameraController->GetTranslation());
        m_Renderer->UploadView({.ProjectionMatrix = m_Camera->GetProjection(), .ViewMatrix = m_Camera->GetView()});
    }
}

void Application::OnEvent(VT::Event& E)
{
    if (VT::Dispatch<VT::WindowCloseEvent>(E, std::bind(&Application::OnWindowClose, this, std::placeholders::_1)))
    {
        return;
    }

    // Temp texture changes
    if (m_Input->IsKeyPressed(VT::Key::C))
    {
        m_CurrentTexture = (m_CurrentTexture + 1) % m_Textures.size();
        TextureSwitch    = true;
    }

    m_Renderer->OnEvent(E);

    /*   for (auto Layer : m_LayerStack)
       {
           Layer->OnEvent(E);
           if (E.Handled())
           {
               break;
           }
       }*/

    // VT_TRACE("{}", E);
}

bool Application::OnWindowClose(VT::WindowCloseEvent&)
{
    m_Running = false;
    return true;
}

Application::~Application()
{
    for (VT::Texture* i : m_Textures)
    {
        delete i;
    }
}
