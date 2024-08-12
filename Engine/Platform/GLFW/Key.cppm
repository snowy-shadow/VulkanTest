#include <type_traits>
export module VT.Platform.GLFW.Key;

import VT.Key;

export namespace VT::GLFW
{
int ToKeyCode(VT::Key Key) { return static_cast<typename std::underlying_type<VT::Key>::type>(Key); }

VT::Key FromKeyCode(int GLFW_Key) { return static_cast<VT::Key>(GLFW_Key); }
} // namespace VT::GLFW