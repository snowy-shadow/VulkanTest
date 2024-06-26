module;
#include "VT_Export"
export module VT.Timestep;

export namespace VT
{
// TODO : update to tick system instead
class VT_ENGINE_EXPORT Timestep
{
public:
    Timestep(float Time = 0.f) : m_TimeSecond(Time) {}

    inline float GetSecond() { return m_TimeSecond; }
    inline float GetMiliSecond() { return m_TimeSecond * 1000.f; }

    operator float() const { return m_TimeSecond; }

private:
    float m_TimeSecond;
};
} // namespace VT
