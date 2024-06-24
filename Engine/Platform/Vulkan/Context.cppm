export module VT.Platform.Vulkan.Context;

import VT.RendererContext;
import VT.Util;
import VT.Window;

export namespace VT::Vulkan
{
class Context : public RendererContext
{
public:
    Context(Shared<Window> Window);
    void Init() override;
    void SwapBuffers() override;

private:
    Shared<Window> m_Window;
};
} // namespace VT::Vulkan
