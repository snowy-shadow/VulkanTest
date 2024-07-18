module;
#include "VT_Export"
export module VT.Renderer;

import VT.Texture;
import VT.RendererType;
import VT.RendererContext;
import VT.Window;
import VT.Event;
import VT.Camera;
import VT.Util;
import VT.Timestep;

export namespace VT
{
class Renderer
{
public:
    VT_ENGINE_EXPORT Renderer(RendererType::API Type = RendererType::API::eVulkan, Shared<Window> Window = nullptr);

    VT_ENGINE_EXPORT void BeginScene(); // Shared<Camera> Camera);
    VT_ENGINE_EXPORT void EndScene();

    VT_ENGINE_EXPORT void BeginRenderPass();
    VT_ENGINE_EXPORT void EndRenderPass();

    VT_ENGINE_EXPORT Uniq<Texture> CreateTexture(const TextureCreateInfo& TextureInfo);

    VT_ENGINE_EXPORT void Submit();

    VT_ENGINE_EXPORT void OnUpdate(const Timestep& Time);
    VT_ENGINE_EXPORT void OnEvent(Event& E);

    VT_ENGINE_EXPORT void SetRendererAPI(RendererType::API Type, Shared<Window> Window);

private:
    inline static Uniq<RendererContext> CreateAPI(RendererType::API Type, Shared<Window> Window);

private:
    Uniq<RendererContext> m_API;
};
} // namespace VT
