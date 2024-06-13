module;
#include <GLFW/glfw3.h>
#include <functional>

export module Platform.Windows.Window;
import titw;

namespace VT::Windows
{
class Window : public ::VT::Window
{
public:
    Window(const ::VT::WindowProperties& Properties = ::VT::WindowProperties {});
    void OnUpdate() override;

    unsigned int GetWidth() const override;
    unsigned int GetHeight() const override;

    void SetEventCallBack(const std::function<void(Event&)> CallbackFN) override { m_Data.EventCallBack = CallBackFN; }

    void SetVSync(bool Enabled) override;
    bool IsVSync() const override;

    ~Window();

private:
    GLFWwindow* m_Window;

    struct WindowInfo : ::VT::WindowProperties
    {
        bool VSync;

        std::function<void(Event&)> EventCallBack;
    } m_Data;

    static bool m_GLFW_Initialized;
}
} // namespace VT::Windows
