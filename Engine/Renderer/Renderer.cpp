module VT.Renderer;

import VT.RendererOption;
import VT.RenderCmdAPI;

namespace VT
{

Renderer::Renderer(RendererAPI Type) : m_API(SetAPI(Type)) {}

void Renderer::BeginScene(Camera* Camera) {}
void Renderer::EndScene() {}

void Renderer::Submit() {}

void Renderer::SetRendererAPI(RendererAPI API) { m_API = SetAPI(API); }

inline RenderCmdAPI* Renderer::SetAPI(RendererAPI Type)
{
    delete m_API;
    return nullptr;
}
} // namespace VT
