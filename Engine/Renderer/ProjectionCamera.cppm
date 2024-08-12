module;
#include "VT_Export"
#include <glm/glm.hpp>
export module VT.ProjectionCamera;

import VT.Camera;
import VT.Event;

export namespace VT
{
class VT_ENGINE_EXPORT ProjectionCamera final : public Camera
{
public:
    ProjectionCamera(float X, float Y, unsigned int FOV = 90);

    virtual glm::mat4 GetProjection() override;
    virtual glm::mat4 GetView() override;
    virtual glm::mat4 GetViewProjection() override;

    void Resize(float X, float Y);
    void OnEvent(Event& Event);

    glm::vec3 GetPosition() const { return m_Translation; }
    glm::vec3 GetRotation() const { return m_Rotation; }

    void SetRotation(glm::vec3 Rotation);
    void SetTranslation(glm::vec3 Translation);

private:
    void ComputeViewMatrix();

private:
    glm::mat4 m_ProjectionMatrix {1.f};
    glm::mat4 m_ViewMatrix {1.f};

    glm::vec3 m_Rotation {0.f, 0.f, 0.f};
    glm::vec3 m_Translation {0.f, 0.f, 0.f};

    bool m_ValueModified {true};
};
} // namespace VT
