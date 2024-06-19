module VT.Renderer;

import VT.RenderCmdAPI;

namespace VT
{

Renderer::Renderer(RenderCmdAPI::API Type) : m_API(SetAPI(Type)) {}

void Renderer::BeginScene(Camera* Camera) {}
void Renderer::EndScene() {}

void Renderer::Submit() {}

void Renderer::SetRendererAPI(RenderCmdAPI::API API)
{
    delete m_API;
    m_API = SetAPI(API);
}
inline RenderCmdAPI* Renderer::SetAPI(RenderCmdAPI::API Type) { return nullptr; }
} // namespace VT
