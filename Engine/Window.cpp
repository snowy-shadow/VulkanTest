module VT.Window;

import VT.Platform;

namespace VT
{

Window* Window::Create(WindowAPI WindowAPI, RendererOption::API RendererAPI, const WindowProperties& Properties)
{
    switch (API)
    {
        case WindowAPI::eGLFWwindow:
#ifdef _WIN32
            return new Windows::Window(RendererAPI, Properties);
#endif
    }
}
} // namespace VT
