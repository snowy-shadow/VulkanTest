module;
#include "VT_Export"
export module VT.RendererAPI;

export namespace VT
{
enum class RendererAPI
{
    eNone,
    eVulkan
};

class VT_ENGINE_EXPORT Renderer
{
public:
    constexpr RendererAPI GetAPI() { return m_RendererAPI; }

private:
    RendererAPI m_RendererAPI;
};
} // namespace VT
