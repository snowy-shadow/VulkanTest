module;
#include "VT_Export"
#include <glm/glm.hpp>
export module VT.OrthographicCamera;

import Camera3D;

namespace VT
{
class VT_ENGINE_EXPORT OrthographicCamera final : public Camera,
{
public:
    OrthographicCamera() = default;
    OrthographicCamera(float Left, float Right, float Top, float Bottom, unsigned int FOV = 90);

    glm::mat4 GetProjection() override;
    glm::mat4 GetView() override;
    glm::mat4 GetViewProjection() override;

    Camera3D(glm::vec3 TranslationXYZ, glm::vec3 RotationXYZ) :
        m_TranslationXYZ(TranslationXYZ), m_RotationXYZ(RotationXYZ)
    {
    }

    void SetTranslationXYZ(glm::vec3 TranslationXYZ)
    {
        m_TranslationXYZ = TranslationXYZ;
        m_ValueModified  = true;
    }
    void ApplyTranslationXYZ(glm::vec3 TranslationXYZ)
    {
        m_TranslationXYZ += TranslationXYZ;
        m_ValueModified = true;
    }

    void SetRotationXYZ(glm::vec3 RotationXYZ)
    {
        m_RotationXYZ   = RotationXYZ;
        m_ValueModified = true;
    }
    void ApplyRotationXYZ(glm::vec3 RotationXYZ)
    {
        m_RotationXYZ += RotationXYZ;
        m_ValueModified = true;
    }

    constexpr glm::vec3 GetTranslation_XYZ() const { return m_TranslationXYZ; }
    constexpr glm::vec3 GetRotationRadians_XYZ() const { return m_RotationXYZ; }

private:
    void ComputeViewMatrix();

private:
    glm::mat4 m_ProjectionMatrix {};
    glm::mat4 m_ViewMatrix {};
    glm::mat4 m_ViewProjectionMatrix {};

    glm::vec3 m_TranslationXYZ {0.f};
    // RADIANS
    glm::vec3 m_RotationXYZ {0.f};

    bool m_ValueModified {false};
};
} // namespace VT
