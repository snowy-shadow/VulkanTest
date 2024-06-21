export module VT.RendererContext;

import VT.Window;
import VT.Util;

namespace VT
{
class RendererContext
{
    virtual void Init()        = 0;
    virtual void SwapBuffers() = 0;

    static PtrRef<RendererContext> Create(RendererOption::API API, Window* Window);
    virtual ~RendererContext() = default;
};
} // namespace VT
