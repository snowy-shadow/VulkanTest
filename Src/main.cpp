import VulkanTest;
import Application;

int main()
{
    VT::Engine Engine;
    Engine.Init();

    Application App;

    App.Run();

    Engine.Terminate();
    return 0;
}
