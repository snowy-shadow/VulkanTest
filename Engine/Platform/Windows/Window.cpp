#include <GLFW/glfw3.h>
#include <functional>

import Window;

namespace VT
{
class WindowsWindow : public Window
{
public:
    Window(const WindowProperties& Properties = WindowProperties {});
    void OnUpdate() override;

    unsigned int GetWidth() const override;
    unsigned int GetHeight() const override;

    void SetEventCallBack(const std::function<void(Event&)> CallbackFN) override;

    void SetVSync(bool Enabled) override;
    bool IsVSync() const override;

private:
    virtual void Init(const WindowProperties& Properties);
    virtual void Shutdown();

private:
    GLFWwindow* m_Window;

    struct WindowInfo
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        std::function<void(Event&)> EventCallBack;
    } m_Data;
}
} // namespace VT
