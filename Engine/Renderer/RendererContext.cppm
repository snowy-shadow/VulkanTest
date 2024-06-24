export module VT.RendererContext;

import VT.RendererOption;
import VT.Window;
import VT.Util;

export namespace VT
{
class RendererContext
{
public:
    virtual void Init()        = 0;
    virtual void SwapBuffers() = 0;

    static Shared<RendererContext> Create(RendererOption::API API, Shared<Window> Window);
    virtual ~RendererContext() = default;
};
} // namespace VT
