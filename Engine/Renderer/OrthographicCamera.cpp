module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
module VT.OrthographicCamera;

namespace VT
{
OrthographicCamera::OrthographicCamera(float Left, float Right, float Top, float Bottom, unsigned int FOV) :
    m_ProjectionMatrix(glm::ortho(Left, Right, Bottom, Top, 0.f, 1.f))
{}

glm::mat4 OrthographicCamera::GetProjection() { return m_ProjectionMatrix; }

glm::mat4 OrthographicCamera::GetView()
{
    if (m_ValueModified)
    {
        ComputeViewMatrix();
        m_ValueModified = false;
    }
    return m_ViewMatrix;
}
glm::mat4 OrthographicCamera::GetViewProjection()
{
    if (m_ValueModified)
    {
        ComputeViewMatrix();
        m_ValueModified = false;
    }
    return m_ViewProjectionMatrix;
}

CameraTransform OrthographicCamera::GetTransform()
{
    if (m_ValueModified)
    {
        ComputeViewMatrix();
        m_ValueModified = false;
    }
    return {m_ProjectionMatrix, m_ViewMatrix, m_ViewProjectionMatrix};
}

void OrthographicCamera::ComputeViewMatrix()
{
    const float X = -m_RotationXYZ[0];
    const float Y = -m_RotationXYZ[1];
    const float Z = -m_RotationXYZ[2];

    m_ViewMatrix = glm::translate(glm::mat4(1.f), m_TranslationXYZ) *
        glm::rotate(glm::mat4(1.f), X, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.f), Y, glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1.f), Z, glm::vec3(0, 0, 1));

    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
} // namespace VT
