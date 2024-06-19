module VT.Window;

import VT.Platform;

namespace VT
{

Window* Window::Create(WindowAPI API, const WindowProperties& Properties)
{
    switch (API)
    {
        case WindowAPI::eGLFWwindow:
            return new Windows::Window(Properties);
    }
}
} // namespace VT
