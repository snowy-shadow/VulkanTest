module;
#include <memory>
module VT.Renderer;

namespace VT
{
Renderer::Renderer(GraphicsAPI Type, Shared<Window> Window) : m_API(CreateAPI(Type, std::move(Window)))
{
    m_API->Init();
}

void Renderer::BeginScene() // Shared<Camera> Camera)
{
    // (void) Camera;
    m_API->BeginFrame();
    m_FrameBegan = true;
    VT::GeometryRenderData Data {};

    m_API->UploadGeometry(Data);

    // m_API->BeginScene(Camera);
}
void Renderer::EndScene()
{
    m_FrameBegan = false;
    m_API->EndFrame();
} // m_API->EndScene(); }

Uniq<Texture> Renderer::CreateTexture(const TextureCreateInfo& TextureInfo)
{
    return m_API->CreateTexture(TextureInfo);
}

void Renderer::BeginRenderPass() { VT_CORE_ASSERT(m_FrameBegun, "No frame in progress") }
void Renderer::EndRenderPass() void Renderer::BeginRenderPass() { VT_CORE_ASSERT(m_FrameBegun, "No frame in progress") }

void Renderer::Submit() {}

void Renderer::OnUpdate(const Timestep& Time) { m_API->OnUpdate(Time); }

void Renderer::OnEvent(Event& E) { return m_API->OnEvent(E); }

void Renderer::SetRendererAPI(GraphicsAPI Type, Shared<Window> Window) { m_API = CreateAPI(Type, std::move(Window)); }

Uniq<RendererContext> Renderer::CreateAPI(GraphicsAPI Type, Shared<Window> Window)
{
    return Uniq<RendererContext>(RendererContext::Create(Type, std::move(Window)));
} // namespace VT
} // namespace VT
