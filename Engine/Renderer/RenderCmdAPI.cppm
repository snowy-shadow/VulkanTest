export module VT.RenderCmdAPI;

export namespace VT
{
class RenderCmdAPI
{
public:
    enum class API
    {
        eNone,
        eVulkan
    };

public:
    virtual void SetClearColor() = 0;
    virtual void Clear()         = 0;

    constexpr API GetAPI() const { return m_RendererAPI; }

    virtual ~RenderCmdAPI() = default;

private:
    API m_RendererAPI;
};
} // namespace VT
