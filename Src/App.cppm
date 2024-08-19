module;
#include <array>
export module Application;
import VulkanTest;

export class Application
{
public:
    Application();
    void Run();

    void OnEvent(VT::Event& E);

    ~Application();

private:
    bool OnWindowClose(VT::WindowCloseEvent&);

    void OnUpdate(const VT::Timestep& Ts);

private:
    VT::Shared<VT::Window> m_Window;
    VT::Uniq<VT::Input> m_Input;
    VT::Uniq<VT::ProjectionCamera> m_Camera;
    VT::Uniq<VT::CameraController> m_CameraController;

    VT::Uniq<VT::Renderer> m_Renderer;

    std::array<VT::Texture*, 3> m_Textures;
    uint32_t m_CurrentTexture = 0;
    bool TextureSwitch        = true;


    VT::LayerStack m_LayerStack;
    VT::Timepoint m_TimePoint;

    bool m_Running {true};
};
