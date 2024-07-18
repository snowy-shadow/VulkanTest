module;
#include "VT_Export"
#include <chrono>
export module VT.Timestep;

namespace chrono = std::chrono;

using Duration = chrono::duration;
using Clk      = chrono::high_resolution_clock;

export namespace VT
{
class Timepoint;

// TODO : update to tick system instead
class Timestep
{
public:
    inline double VT_ENGINE_EXPORT Second() const
    {
        return chrono::duration_cast<Duration<double, chrono::seconds>>(m_Timestep).count();
    }
    inline double VT_ENGINE_EXPORT MilliSecond() const
    {
        return chrono::duration_cast<Duration<double, chrono::milliseconds>>(m_Timestep).count();
    }

    Timestep static inline VT_ENGINE_EXPORT abs(const Timestep& Timestep) { return chrono::abs(Timestep.m_Timestep); }

public:
    Timestep inline VT_ENGINE_EXPORT operator-(const Timestep& Other) const { return {m_Timestep - Other.m_Timestep}; }
    Timestep inline VT_ENGINE_EXPORT operator+(const Timestep& Other) const { return {m_Timestep + Other.m_Timestep}; }

public:
    VT_ENGINE_EXPORT Timestep(Clk::duration Time = Clk::duration::zero()) : m_Timestep(Time) {}
    VT_ENGINE_EXPORT Timestep(const Timestep& Other)            = default;
    VT_ENGINE_EXPORT Timestep& operator=(const Timestep& Other) = default;
    VT_ENGINE_EXPORT ~Timestep()                                = default;

private:
    Clk::duration m_Timestep;

    friend Timepoint;
};

class Timepoint
{
public:
    Timepoint static inline VT_ENGINE_EXPORT Now() { return Timepoint {}; }

    /**
     * Get time difference and update internal clock point
     * @param Time Time point to tick to
     * @return Duration difference between current timepoint and new timepoint
     */
    Timestep inline VT_ENGINE_EXPORT Tick(const Timepoint& Time)
    {
        Timestep D  = Time.m_Timepoint - m_Timepoint;
        m_Timepoint = Time.m_Timepoint;
        return D;
    }

    /**
     * Time diff between two points in time
     * @param Time Diff between current time point and other
     * @return
     */
    Timestep inline VT_ENGINE_EXPORT Delta(const Timepoint& Time) const { return {Time.m_Timepoint - m_Timepoint}; }

public:
    Timestep inline VT_ENGINE_EXPORT operator-(const Timepoint& Other) const
    {
        return {m_Timepoint - Other.m_Timepoint};
    }

    Timepoint inline VT_ENGINE_EXPORT operator+(const Timestep& Timestep) const
    {
        return Timepoint(m_Timepoint + Timestep.m_Timestep);
    }
    Timepoint inline VT_ENGINE_EXPORT operator-(const Timestep& Timestep) const
    {
        return Timepoint(m_Timepoint - Timestep.m_Timestep);
    }

public:
    explicit VT_ENGINE_EXPORT Timepoint(Clk::time_point Timepoint = Clk::now()) : m_Timepoint(Timepoint) {}
    VT_ENGINE_EXPORT Timepoint(const Timepoint& Other)            = default;
    VT_ENGINE_EXPORT Timepoint& operator=(const Timepoint& Other) = default;
    VT_ENGINE_EXPORT ~Timepoint()                                 = default;

private:
    Clk::time_point m_Timepoint;
};

} // namespace VT
