#include "EngineMacro.h"
import VulkanTest;

int main()
{
    VT::Engine Engine;
    Engine.Init();

    VT::WindowResizeEvent E {{{1280, 720}}};
    if (E.IsInCategory(VT::EventCategory::EventCategoryApplication))
    {
        VT_TRACE(E.GetName());
    }

    if (E.IsInCategory(VT::EventCategory::EventCategoryInput))
    {
        VT_ERROR("Window Resize is not category input");
    }

    VT::Window* Window = VT::Window::Create();
    Window->OnUpdate();

    VT_INFO("End of program");

    Engine.Terminate();

    delete Window;
    return 0;
}
