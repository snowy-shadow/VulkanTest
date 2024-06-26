export module VT.RendererAPI;

import VT.RendererOption;
export namespace VT
{
class RendererAPI
{
public:
    virtual void Init()     = 0;
    virtual void Shutdown() = 0;

    virtual void SetClearColor(float R, float G, float B, float A) = 0;
    virtual void Clear()                                           = 0;

    constexpr RendererOption::API GetAPI() const { return m_RendererAPI; }

    virtual ~RendererAPI() = default;

private:
    RendererOption::API m_RendererAPI;
};
} // namespace VT
