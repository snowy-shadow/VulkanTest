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
    VT::Uniq<VT::ProjectionCamera> m_Camera;

    VT::Uniq<VT::Renderer> m_Renderer;

    VT::LayerStack m_LayerStack;
    VT::Timepoint m_TimePoint;

    bool m_Running {true};
};
