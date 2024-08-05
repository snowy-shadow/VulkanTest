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
    ProjectionCamera(float Left, float Right, float Top, float Bottom, unsigned int FOV = 90);

    virtual glm::mat4 GetProjection() override;
    virtual glm::mat4 GetView() override;
    virtual glm::mat4 GetViewProjection() override;
    virtual void Resize(float Left, float Right, float Top, float Bottom) override;
    virtual void OnEvent(Event& Event) override;

    virtual CameraTransform GetTransform() override;

private:
    void ComputeViewMatrix();

private:
    glm::mat4 m_ProjectionMatrix {1.f};
    glm::mat4 m_ViewMatrix {1.f};
    glm::mat4 m_ViewProjectionMatrix {1.f};

    bool m_ValueModified {true};
};
} // namespace VT
