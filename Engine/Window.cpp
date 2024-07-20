module VT.Window;

import VT.Platform;

namespace VT
{

Window* Window::Create(WindowAPI WindowAPI, GraphicsAPI RendererAPI, const WindowProperties& Properties)
{
    switch (WindowAPI)
    {
        case WindowAPI::eGLFWwindow:
#ifdef _WIN32
            return new Windows::Window(RendererAPI, Properties);
#endif
    }
}
} // namespace VT
