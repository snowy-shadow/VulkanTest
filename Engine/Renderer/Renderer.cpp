module VT.Renderer;

import VT.RendererOption;
import VT.RendererAPI;
import VT.Window;
import VT.Camera;
import VT.Util;

namespace VT
{

Renderer::Renderer(RendererOption::API Type) : m_API(CreateAPI(Type)) {}

void Renderer::BeginScene(Shared<Camera> Camera) { (void) Camera; }
void Renderer::EndScene() {}

void Renderer::Submit() {}

void Renderer::SetRendererAPI(RendererOption::API Type) { m_API = CreateAPI(Type); }

inline Uniq<RendererAPI> Renderer::CreateAPI(RendererOption::API Type)
{
    switch (Type)
    {
        // case RendererOption::API::eVulkan:
        //     return CreateRef < Vulkan::Context
    }
    return nullptr;
}
} // namespace VT
