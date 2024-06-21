module;
#include <GLFW/glfw3.h>
#include "EngineMacro.h"

module VT.Vulkan.Context;
import VT.Log;

namespace VT::Vulkan
{
Context::Context(Window* Window) : m_Window(Window) {}
void Context::Init()
{
    if (Window->GetWindowAPI() == WindowAPI::eGLFWwindow)
    {
        VT_CORE_ASSERT(glfwVulkanSupported(), "GLFW Window context does not support vulkan");
    }
}
void Context::SwapBuffers() {}

} // namespace VT::Vulkan