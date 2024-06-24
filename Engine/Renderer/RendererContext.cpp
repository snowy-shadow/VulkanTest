module;
#include "EngineMacro.h"
module VT.RendererContext;

import VT.RendererOption;
import VT.Platform;
import VT.Log;

namespace VT
{

Shared<RendererContext> RendererContext::Create(RendererOption::API API, Shared<Window> Window)
{
    switch (API)
    {
        case RendererOption::API::eVulkan:
            return CreateRef<Vulkan::Context>(Window);
    }
    VT_CORE_HALT("Unsupported RendererAPI");

    return nullptr;
}
} // namespace VT
