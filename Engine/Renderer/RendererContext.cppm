module;
#include "VT_Export"
export module VT.RendererContext;

import VT.RendererType;
import VT.Event;
import VT.Window;
import VT.Util;
import VT.Timestep;

export namespace VT
{
class VT_ENGINE_EXPORT RendererContext
{
public:
    virtual void Init() = 0;

    virtual bool BeginFrame() = 0;
    virtual bool EndFrame() = 0;

    virtual Uniq<Texture> CreateTexture(const TextureCreateInfo& TextureInfo) = 0;
    virtual void UploadView(UniformCameraData Data)                           = 0;
    virtual void UploadGeometry(GeometryRenderData Data)                      = 0;

    virtual void OnUpdate(const Timestep& Time) = 0;
    virtual void OnEvent(Event& Event) = 0;

    static RendererContext* Create(GraphicsAPI API, Shared<Window> Window);
    virtual ~RendererContext() = default;
};
} // namespace VT
