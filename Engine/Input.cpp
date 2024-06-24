module;
#include "EngineMacro.h"

module VT.Input;

import VT.Window;
import VT.Util;
import VT.Platform;
import VT.Log;

namespace VT
{
Uniq<Input> Input::Create(const Window& Window)
{
    switch (Window.GetWindowAPI())
    {
        case WindowAPI::eGLFWwindow:
        {
            return CreateUniq<GLFW::Input>(Window.GetNativeWindow());
        }

        default:
            VT_CORE_HALT("Failed to create Input handler : Invalid Window API");
            nullptr;
    }
}
} // namespace VT
