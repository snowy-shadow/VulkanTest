module;
#include <GLFW/glfw3.h>
#include <utility>
module VT.Platform.GLFW.Input;

import VT.Window;

namespace VT::GLFW
{
Input::Input(void* Window) : m_Window(static_cast<GLFWwindow*>(Window)) {}

bool Input::IsKeyPressed(int KeyCode)
{
    auto State = glfwGetKey(m_Window, KeyCode);
    return State == GLFW_PRESS || State == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(int Button) { return glfwGetMouseButton(m_Window, Button) == GLFW_PRESS; }

std::pair<float, float> Input::GetMouseXY()
{
    double X, Y;
    glfwGetCursorPos(m_Window, &X, &Y);

    return {static_cast<float>(X), static_cast<float>(Y)};
}
} // namespace VT::GLFW
