module;
#include "EngineMacro.h"
module VT.RendererContext;

import VT.Platform;
import VT.Log;
import VT.Platform.Vulkan.Context;

namespace VT
{
RendererContext* RendererContext::Create(GraphicsAPI API, Shared<Window> Window)
{
    switch (API)
    {
        case GraphicsAPI::eVulkan:
            return new Vulkan::RendererContext(std::move(Window));
    }
    VT_CORE_HALT("Unsupported RendererAPI");

    return nullptr;
}
} // namespace VT
