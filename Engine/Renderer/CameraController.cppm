module;
#include <glm/glm.hpp>

#include "VT_Export"

export module VT.CameraController;
import VT.Camera;
import VT.Event;
import VT.Timestep;
import VT.Input;

export namespace VT
{
class VT_ENGINE_EXPORT CameraController
{
public:
    void BindInput(const Input* Input);
    void OnUpdate(Timestep Ts);
    void OnEvent(Event& E);

    void Left(float Unit);
    void Right(float Unit);
    void Up(float Unit);
    void Down(float Unit);

    void SetTranslation(glm::vec3 Translation);
    void SetRotation(glm::vec3 Rotation);

    void ApplyRotation(glm::vec3 XYZ);
    void ApplyTranslation(glm::vec3 XYZ);

    constexpr glm::vec3 GetTranslation() const { return m_Translation; }
    constexpr glm::vec3 GetRotationRadians() const { return m_Rotation; }

private:
    const Input* m_Input {nullptr};
    glm::vec3 m_Translation {0.f, 0.f, 0.f};
    // RADIANS
    glm::vec3 m_Rotation { 0.f, 0.f, 0.f};
};
} // namespace VT
