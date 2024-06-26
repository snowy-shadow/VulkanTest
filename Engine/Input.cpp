module;
#include "EngineMacro.h"

module VT.Input;

import VT.Window;
import VT.Util;
import VT.Platform;
import VT.Log;

namespace VT
{
Input* Input::Create(const Window& Window)
{
    switch (Window.GetWindowAPI())
    {
        case WindowAPI::eGLFWwindow:
        {
            return new GLFW::Input(Window.GetNativeWindow());
        }

        default:
            VT_CORE_HALT("Failed to create Input handler : Invalid Window API");
            return nullptr;
    }
}
} // namespace VT
