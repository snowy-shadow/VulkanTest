module;
#include <glm/glm.hpp>
#include "VT_Export"

export module VT.Camera;

import VT.Event;

export namespace VT
{
struct VT_ENGINE_EXPORT CameraTransform
{
    glm::mat4 ProjectionMatrix {};
    glm::mat4 ViewMatrix {};
    glm::mat4 ViewProjectionMatrix {};
};

class VT_ENGINE_EXPORT Camera
{
public:
    virtual glm::mat4 GetProjection()     = 0;
    virtual glm::mat4 GetView()           = 0;
    virtual glm::mat4 GetViewProjection() = 0;
    virtual CameraTransform GetTransform() = 0;

    virtual void Resize(float Left, float Right, float Top, float Bottom) = 0;
    virtual void OnEvent(Event& Event)                                    = 0;

    virtual ~Camera() = default;
};
} // namespace VT
