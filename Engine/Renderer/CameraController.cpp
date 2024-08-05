module;
#include <glm/glm.hpp>
module VT.CameraController;

namespace VT
{
void CameraController::Left(float Unit);
void CameraControlle::Right(float Unit);
void CameraControlle::Up(float Unit);
void CameraControlle::Down(float Unit);
void CameraController::SetTranslation(glm::vec3 TranslationXYZ)
{
    m_Translation   = Translation;
    m_ValueModified = true;
}

void CameraController::SetRotation(glm::vec3 Rotation)
{
    m_Rotation      = Rotation;
    m_ValueModified = true;
}
void CameraControlle::ApplyRotation(glm::vec3 Rotation)
{
    m_Rotation += Rotation;
    m_ValueModified = true;
}
void CameraControlle::ApplyTranslation(glm::vec3 Translation)
{
    m_Translation += Translation;
    m_ValueModified = true;
}
} // namespace VT
