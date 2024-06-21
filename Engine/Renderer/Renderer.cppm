module;
#include "VT_Export"
export module VT.Renderer;

import VT.RendererOption;
import VT.RenderCmdAPI;
import VT.Camera;

export namespace VT
{
class VT_ENGINE_EXPORT Renderer
{
public:
    Renderer(RendererOption::API Type = RendererOption::API::eVulkan);

    void BeginScene(Camera* Camera);
    void EndScene();

    void BeginRenderPass();
    void EndRenderPass();

    void Submit();

    void SetRendererAPI(RendererOption::API API);

private:
    static inline RenderCmdAPI* SetAPI(RendererAPI Type);

private:
    RenderCmdAPI* m_API;
};
} // namespace VT
