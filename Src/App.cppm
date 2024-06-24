module;
#include <memory>

export module Application;
import VulkanTest;

export class Application
{
public:
    Application();
    void Run();

    void OnEvent(VT::Event& E);

private:
    bool OnWindowClose(VT::WindowCloseEvent&);

private:
    VT::Shared<VT::Window> m_Window;
    VT::Uniq<VT::Input> m_Input;

    VT::LayerStack m_LayerStack;

    bool m_Running {true};
};
