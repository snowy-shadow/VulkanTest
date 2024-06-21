export module VT.Vulkan.Context;

import VT.RendererContext;
import VT.Window;

namespace VT::Vulkan
{
class Context : public RendererContext
{
public:
    Context(Window* Window);
    void Init() override;
    void SwapBuffers() override;

private:
    Window* m_Window {nullptr};
};
} // namespace VT::Vulkan
