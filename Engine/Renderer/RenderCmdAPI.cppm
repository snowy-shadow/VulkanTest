export module VT.RenderCmdAPI;

import VT.RendererOption;
export namespace VT
{
class RenderCmdAPI
{
public:
    virtual void SetClearColor() = 0;
    virtual void Clear()         = 0;

    constexpr RendererOption::API GetAPI() const { return m_RendererAPI; }

    virtual ~RenderCmdAPI() = default;

private:
    RendererOption::API m_RendererAPI;
};
} // namespace VT
