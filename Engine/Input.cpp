module;
#include "EngineMacro.h"

module VT.Input;

import VT.Window;
import VT.Platform;
import VT.Log;

namespace VT
{
Input& Input::Create(const Window& Window)
{
    switch (Window.GetWindowAPI())
    {
        case WindowAPI::eGLFWwindow:
        {
            static GLFW::Input Input {Window.GetNativeWindow()};
            return Input;
        }

        default:
            VT_CORE_HALT("Failed to create Input handler : Invalid Window API");
    }
}
} // namespace VT
