module;
#include <GLFW/glfw3.h>
#include <utility>
export module VT.Platform.GLFW.Input;

import VT.Input;
import VT.Window;

export namespace VT::GLFW
{
class Input final : public VT::Input
{
public:
    Input(void* Window);

    bool IsKeyPressed(int KeyCode) override;
    bool IsMouseButtonPressed(int Button) override;
    std::pair<float, float> GetMouseXY() override;

private:
    GLFWwindow* m_Window;
};
} // namespace VT::GLFW
