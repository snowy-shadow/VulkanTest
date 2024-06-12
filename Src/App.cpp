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

    Engine.Terminate();
    return 0;
}
