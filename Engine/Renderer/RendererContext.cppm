module;
#include "VT_Export"
export module VT.RendererContext;

import VT.RendererOption;
import VT.Window;
import VT.Util;

export namespace VT
{
class VT_ENGINE_EXPORT RendererContext
{
public:
    virtual void Init() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame()   = 0;

    static RendererContext* Create(RendererOption::API API, Shared<Window> Window);
    virtual ~RendererContext() = default;
};
} // namespace VT
