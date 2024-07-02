module VT.Renderer;

import VT.Event;

namespace VT
{

Renderer::Renderer(RendererOption::API Type) : m_API(CreateAPI(Type)) {}

void Renderer::BeginScene(Shared<Camera> Camera)
{
    (void) Camera;
    // m_API->BeginScene(Camera);
}
void Renderer::EndScene() {} // m_API->EndScene(); }

void Renderer::BeginRenderPass() {}
void Renderer::EndRenderPass() {}
void Renderer::Submit() {}

void Renderer::SetRendererAPI(RendererOption::API Type) { m_API = CreateAPI(Type); }

inline Uniq<RendererContext> Renderer::CreateAPI(RendererOption::API Type)
{
    switch (Type)
    {
        // case RendererOption::API::eVulkan:
        //     return CreateRef < Vulkan::Context
    }
    return nullptr;
}
} // namespace VT
