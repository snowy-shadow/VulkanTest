module;
#include <glm/glm.hpp>
#include "VT_Export"

export module VT.Camera;

export namespace VT
{
class VT_ENGINE_EXPORT Camera
{
public:
    virtual glm::mat4 GetProjection()     = 0;
    virtual glm::mat4 GetView()           = 0;
    virtual glm::mat4 GetViewProjection() = 0;

    virtual ~Camera() = default;
};
} // namespace VT
