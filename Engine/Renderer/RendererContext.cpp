module;
#include "EngineMacro.h"
module VT.RendererContext;

import VT.Platform;
import VT.Log;
import VT.Platform.Vulkan.Context;

namespace VT
{
RendererContext* RendererContext::Create(RendererType::API API, Shared<Window> Window)
{
    switch (API)
    {
        case RendererType::API::eVulkan:
            return new Vulkan::RendererContext(std::move(Window));
    }
    VT_CORE_HALT("Unsupported RendererAPI");

    return nullptr;
}
} // namespace VT
