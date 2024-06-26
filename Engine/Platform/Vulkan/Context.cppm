module;
#include <GLFW/glfw3.h>

#include <compare>
#include <vulkan/vulkan.hpp>

#include "EngineMacro.h"

export module VT.Platform.Vulkan.Context;

import VT.Platform.Vulkan.Instance;
import VT.Platform.Vulkan.PhysicalDevice;

import VT.RendererContext;
import VT.Util;
import VT.Window;

export namespace VT::Vulkan
{
class Context : public RendererContext
{
public:
    Context(Shared<Window> Window);
    ~Context();

public:
    void Init() override;
    void SwapBuffers() override;

private:
    Shared<Window> m_Window;

    Instance m_Instance;
    PhysicalDevice m_PhysicalDevice;
    vk::Device m_LogicalDevice;
    vk::SurfaceKHR m_Surface;

    vk::CommandPool m_CmdPool;
};
} // namespace VT::Vulkan
