module;
#include <glm/glm.hpp>
#include "EngineMacro.h"
module VT.CameraController;

import VT.Log;

namespace VT
{
void CameraController::BindInput(const Input* Input) { m_Input = Input; }

void CameraController::OnUpdate(Timestep Ts)
{
    if (m_Input->IsKeyPressed(Key::W))
    {
        Up(static_cast<float>(Ts.Second()) * 0.5f);
    }
    if (m_Input->IsKeyPressed(Key::S))
    {
        Down(static_cast<float>(Ts.Second()) * 0.5f);
    }

    if (m_Input->IsKeyPressed(Key::A))
    {
        Left(static_cast<float>(Ts.Second()) * 0.5f);
    }
    if (m_Input->IsKeyPressed(Key::D))
    {

        Right(static_cast<float>(Ts.Second()) * 0.5f);
    }
}

void CameraController::OnEvent(Event& E)
{
    //switch (Event.GetEventType())
    //{
    //     case EventType::eWindowResize:
    //    {
    //         auto& E = dynamic_cast<WindowResizeEvent&>(Event);
    //         Resize(0.f, E.GetWidth(), E.GetHeight(), 0.f);
    //         break;
    //     }
    //}
}

void CameraController::Left(float Unit)
{
    ApplyTranslation(glm::vec3(Unit, 0.f, 0.f));
    VT_CORE_TRACE("Translation applied");
}

void CameraController::Right(float Unit)
{
    ApplyTranslation(glm::vec3(-Unit, 0.f, 0.f));
    VT_CORE_TRACE("Translation applied");
}
void CameraController::Up(float Unit)
{
    ApplyTranslation(glm::vec3(0.f, -Unit, 0.f));
    VT_CORE_TRACE("Translation applied");
}
void CameraController::Down(float Unit)
{
    ApplyTranslation(glm::vec3(0.f, Unit, 0.f));
    VT_CORE_TRACE("Translation applied");
}

void CameraController::SetTranslation(glm::vec3 XYZ)
{
    m_Translation   = XYZ;
}

void CameraController::SetRotation(glm::vec3 XYZ)
{
    m_Rotation      = XYZ;
}
void CameraController::ApplyRotation(glm::vec3 XYZ)
{
    m_Rotation += XYZ;
}
void CameraController::ApplyTranslation(glm::vec3 XYZ)
{
    m_Translation += XYZ;
}

} // namespace VT
