module VT.Renderer;

import VT.RendererOption;
import VT.RenderCmdAPI;
import VT.Camera;
import VT.Util;

namespace VT
{

Renderer::Renderer(RendererOption::API Type) : m_API(SetAPI(Type)) {}

void Renderer::BeginScene(Shared<Camera> Camera) { (void) Camera; }
void Renderer::EndScene() {}

void Renderer::Submit() {}

void Renderer::SetRendererAPI(RendererOption::API API) { m_API = SetAPI(API); }

inline Shared<RenderCmdAPI> Renderer::SetAPI(RendererOption::API Type)
{
    m_API.reset();
    // TODO : Set type by API
    return m_API;
}
} // namespace VT
