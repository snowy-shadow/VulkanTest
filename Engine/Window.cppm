module;
#include <functional>
#include "VT_Export"
export module VT.Window;

import VT.Event;
import VT.RendererType;

export namespace VT
{

struct VT_ENGINE_EXPORT WindowProperties
{
    const char* Title;
    unsigned int Width;
    unsigned int Height;

    WindowProperties(const char* Title = "Title", unsigned int Width = 1280, unsigned int Height = 720) :
        Title(Title), Width(Width), Height(Height)
    {
    }
};

enum class WindowAPI
{
    eGLFWwindow
};

class VT_ENGINE_EXPORT Window
{
public:
    static Window* Create(
        WindowAPI WindowAPI                = WindowAPI::eGLFWwindow,
                          RendererType::API RendererAPI      = RendererType::API::eVulkan,
        const WindowProperties& Properties = WindowProperties {});

    virtual void OnUpdate() = 0;

    virtual unsigned int GetWidth() const  = 0;
    virtual unsigned int GetHeight() const = 0;

    virtual void SetEventCallBack(const std::function<void(Event&)> CallBack) = 0;

    virtual void SetVSync(bool Enabled) = 0;
    virtual bool IsVSync() const        = 0;

    virtual constexpr WindowAPI GetWindowAPI() const = 0;
    virtual void* GetNativeWindow() const            = 0;

    virtual ~Window() = default;
};
} // namespace VT
