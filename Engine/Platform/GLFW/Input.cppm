module;
#include <GLFW/glfw3.h>
#include <utility>
export module VT.Platform.GLFW.Input;

import VT.Input;
import VT.Window;
import VT.Key;

export namespace VT::GLFW
{
class Input final : public VT::Input
{
public:
    Input(void* Window);

    bool IsKeyPressed(Key KeyCode) const override;
    bool IsMouseButtonPressed(Key Button) const override;
    std::pair<float, float> GetMouseXY() const override;

private:
    GLFWwindow* m_Window;
};
} // namespace VT::GLFW
