module;
#include "VT_Export"
export module VT.Renderer;

import VT.RendererOption;
import VT.RenderCmdAPI;
import VT.Camera;
import VT.Util;

export namespace VT
{
class VT_ENGINE_EXPORT Renderer
{
public:
    Renderer(RendererOption::API Type = RendererOption::API::eVulkan);

    void BeginScene(Shared<Camera> Camera);
    void EndScene();

    void BeginRenderPass();
    void EndRenderPass();

    void Submit();

    void SetRendererAPI(RendererOption::API API);

private:
    inline Shared<RenderCmdAPI> SetAPI(RendererOption::API Type);

private:
    Shared<RenderCmdAPI> m_API;
};
} // namespace VT
