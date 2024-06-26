module;
#include "VT_Export"
export module VT.Renderer;

import VT.RendererOption;
import VT.RendererAPI;
import VT.Window;
import VT.Camera;
import VT.Util;

export namespace VT
{
class Renderer
{
public:
    VT_ENGINE_EXPORT Renderer(RendererOption::API Type = RendererOption::API::eVulkan);

    VT_ENGINE_EXPORT void BeginScene(Shared<Camera> Camera);
    VT_ENGINE_EXPORT void EndScene();

    VT_ENGINE_EXPORT void BeginRenderPass();
    VT_ENGINE_EXPORT void EndRenderPass();

    VT_ENGINE_EXPORT void Submit();

    VT_ENGINE_EXPORT void SetRendererAPI(RendererOption::API Type);

private:
    inline static Uniq<RendererAPI> CreateAPI(RendererOption::API Type);

private:
    Uniq<RendererAPI> m_API;
};
} // namespace VT
