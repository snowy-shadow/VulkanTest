module;
#include "EngineMacro.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
module VT.ProjectionCamera;

import VT.Log;

namespace VT
{
ProjectionCamera::ProjectionCamera(float Left, float Right, float Top, float Bottom, unsigned int FOV) :
    m_ProjectionMatrix(glm::perspective<float>(glm::radians(45.f), 1280 / 720, 0.1f, 1000.0f))
{
    // m_ProjectionMatrix[1][1] *= -1;
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
    return m_ViewProjectionMatrix;
}

void ProjectionCamera::Resize(float Left, float Right, float Top, float Bottom)
{
    VT_CORE_TRACE("OrthographicCamera::Resize({0}, {1}, {2}, {3}) aspect ratio : {4}",
                  Left,
                  Right,
                  Top,
                  Bottom,
                  abs(Left - Right) / abs(Top - Bottom));
    m_ProjectionMatrix =
        glm::perspective<float>(glm::radians(45.f), abs(Left - Right) / abs(Top - Bottom), 0.1f, 1000.0f);
    // m_ProjectionMatrix[1][1] *= -1;
    ComputeViewMatrix();
}

void ProjectionCamera::OnEvent(Event& Event)
{
    switch (Event.GetEventType())
    {
        // case EventType::eWindowResize:
        //{
        //     auto& E = dynamic_cast<WindowResizeEvent&>(Event);
        //     Resize(0.f, E.GetWidth(), E.GetHeight(), 0.f);
        //     break;
        // }
        case EventType::eKeyPress:
        {
            auto& E = dynamic_cast<KeyPressEvent&>(Event);
            if (E.GetKeyCode() == GLFW_KEY_W)
            {
                ApplyTranslationXYZ(glm::vec3(0.f, 0.f, 0.5f));
            }
            if (E.GetKeyCode() == GLFW_KEY_S)
            {
                ApplyTranslationXYZ(glm::vec3(0.f, 0.f, -0.5f));
            }
            if (E.GetKeyCode() == GLFW_KEY_A)
            {
                ApplyTranslationXYZ(glm::vec3(-0.5f, 0.f, 0.f));
            }
            if (E.GetKeyCode() == GLFW_KEY_D)
            {
                ApplyTranslationXYZ(glm::vec3(0.5f, 0.f, 0.f));
            }
            m_ValueModified = true;
            VT_CORE_TRACE("Translation applied");
            break;
        }
    }
}

CameraTransform ProjectionCamera::GetTransform()
{
    if (m_ValueModified)
       {
           ComputeViewMatrix();
           m_ValueModified = false;
       }
    return {m_ProjectionMatrix, m_ViewMatrix, m_ViewProjectionMatrix};
}

void ProjectionCamera::ComputeViewMatrix()
{
    const float X = m_RotationXYZ[0];
    const float Y = m_RotationXYZ[1];
    const float Z = m_RotationXYZ[2];

    m_ViewMatrix = glm::translate(glm::mat4(1.f), m_TranslationXYZ); /**
        (glm::rotate(glm::mat4(1.f), X, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.f), Y, glm::vec3(0, 1, 0)) *
         glm::rotate(glm::mat4(1.f), Z, glm::vec3(0, 0, 1)));*/

    m_ViewMatrix = glm::inverse(m_ViewMatrix);

    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
} // namespace VT
