module;
#include "VT_Export"
export module VT.Renderer;

import VT.RenderCmdAPI;
import VT.Camera;

export namespace VT
{
class VT_ENGINE_EXPORT Renderer
{
public:
    Renderer(RenderCmdAPI::API Type = RenderCmdAPI::API::eVulkan);

    void BeginScene(Camera* Camera);
    void EndScene();

    void BeginRenderPass();
    void EndRenderPass();

    void Submit();

    void SetRendererAPI(RenderCmdAPI::API API);

private:
    static inline RenderCmdAPI* SetAPI(RenderCmdAPI::API Type);

private:
    RenderCmdAPI* m_API;
};
} // namespace VT
