module;
#include <GLFW/glfw3.h>
#include <utility>
module VT.Platform.GLFW.Input;

import VT.Window;
import VT.Platform.GLFW.Key;

namespace VT::GLFW
{
Input::Input(void* Window) : m_Window(static_cast<GLFWwindow*>(Window)) {}

bool Input::IsKeyPressed(Key KeyCode) const
{
    auto State = glfwGetKey(m_Window, GLFW::ToKeyCode(KeyCode));
    return State == GLFW_PRESS || State == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(Key Button) const
{
    return glfwGetMouseButton(m_Window, GLFW::ToKeyCode(Button)) == GLFW_PRESS;
}

std::pair<float, float> Input::GetMouseXY() const
{
    double X, Y;
    glfwGetCursorPos(m_Window, &X, &Y);

    return {static_cast<float>(X), static_cast<float>(Y)};
}
} // namespace VT::GLFW
