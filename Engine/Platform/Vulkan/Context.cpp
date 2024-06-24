module;
#include <GLFW/glfw3.h>
#include "EngineMacro.h"

module VT.Platform.Vulkan.Context;
import VT.Log;
import VT.Util;

namespace VT::Vulkan
{
Context::Context(Shared<Window> Window) : m_Window(Window) {}
void Context::Init()
{
    if (m_Window->GetWindowAPI() == WindowAPI::eGLFWwindow)
    {
        VT_CORE_ASSERT(glfwVulkanSupported(), "GLFW Window context does not support vulkan");
    }
}
void Context::SwapBuffers() {}

} // namespace VT::Vulkan
