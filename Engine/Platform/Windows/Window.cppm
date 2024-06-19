module;
#include <GLFW/glfw3.h>
#include <functional>

export module VT.Platform.Windows.Window;

import VT.Window;
import VT.Event;

export namespace VT::Windows
{
class Window final : public VT::Window
{
public:
    Window(const VT::WindowProperties& Properties = VT::WindowProperties {});
    void OnUpdate() override;

    unsigned int GetWidth() const override;
    unsigned int GetHeight() const override;

    void SetEventCallBack(const std::function<void(Event&)> Callback) override { m_Data.EventCallBack = Callback; }

    void SetVSync(bool Enable) override;
    bool IsVSync() const override;

    constexpr WindowAPI GetWindowAPI() const override { return WindowAPI::eGLFWwindow; };
    inline void* GetNativeWindow() const override { return m_Window; }

    ~Window();

private:
    GLFWwindow* m_Window;

    struct WindowInfo : VT::WindowProperties
    {
        bool VSync;

        std::function<void(Event&)> EventCallBack;
    } m_Data;
};
} // namespace VT::Windows
