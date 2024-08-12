module;
#include "EngineMacro.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
module VT.ProjectionCamera;

import VT.Log;

namespace VT
{
ProjectionCamera::ProjectionCamera(float X, float Y, unsigned int FOV) :
    m_ProjectionMatrix(glm::perspective<float>(glm::radians(45.f), 1280 / 720, 0.1f, 1000.0f))
{
}

glm::mat4 ProjectionCamera::GetProjection() { return m_ProjectionMatrix; }
glm::mat4 ProjectionCamera::GetView()
{
    if (m_ValueModified)
    {
        ComputeViewMatrix();
        m_ValueModified = false;
    }
    return m_ViewMatrix;
}
glm::mat4 ProjectionCamera::GetViewProjection()
{
    if (m_ValueModified)
    {
        ComputeViewMatrix();
        m_ValueModified = false;
    }

    return m_ProjectionMatrix * m_ViewMatrix;
}

void ProjectionCamera::SetTranslation(glm::vec3 Translation) { m_Translation = Translation; }

void ProjectionCamera::SetRotation(glm::vec3 Rotation) { m_Rotation = Rotation; }

void ProjectionCamera::Resize(float X, float Y)
{
    m_ProjectionMatrix =
        glm::perspective<float>(glm::radians(45.f), X / Y, 0.1f, 1000.0f);
}

void ProjectionCamera::OnEvent(Event& Event)
{
    switch (Event.GetEventType())
    {
        case EventType::eWindowResize:
        {
            auto Dimension = static_cast<WindowResizeEvent&>(Event).GetDimensionXY();
            Resize(Dimension[0], Dimension[1]);
            break;
        }

    }
}

void ProjectionCamera::ComputeViewMatrix()
{
    // const float X = m_Rotation[0];
    // const float Y = m_Rotation[1];
    // const float Z = m_Rotation[2];

    m_ViewMatrix = glm::translate(glm::mat4(1.f), m_Translation); /**
        (glm::rotate(glm::mat4(1.f), X, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.f), Y, glm::vec3(0, 1, 0)) *
         glm::rotate(glm::mat4(1.f), Z, glm::vec3(0, 0, 1)));*/

    m_ViewMatrix = glm::inverse(m_ViewMatrix);
}
} // namespace VT
