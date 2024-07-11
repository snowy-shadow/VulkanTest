module;
#include <memory>
module VT.Renderer;

namespace VT
{
Renderer::Renderer(RendererOption::API Type, Shared<Window> Window) : m_API(CreateAPI(Type, std::move(Window)))
{
    m_API->Init();
}

void Renderer::BeginScene() // Shared<Camera> Camera)
{
    // (void) Camera;
    m_API->BeginFrame();
    // m_API->BeginScene(Camera);
}
void Renderer::EndScene() { m_API->EndFrame(); } // m_API->EndScene(); }

void Renderer::BeginRenderPass() {}
void Renderer::EndRenderPass() {}
void Renderer::Submit() {}

void Renderer::OnEvent(Event& E) { return m_API->OnEvent(E); }

void Renderer::SetRendererAPI(RendererOption::API Type, Shared<Window> Window)
{
    m_API = CreateAPI(Type, std::move(Window));
}

Uniq<RendererContext> Renderer::CreateAPI(RendererOption::API Type, Shared<Window> Window)
{
    return Uniq<RendererContext>(RendererContext::Create(Type, std::move(Window)));
} // namespace VT
} // namespace VT
