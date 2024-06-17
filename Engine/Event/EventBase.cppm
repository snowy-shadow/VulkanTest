module;
#include "VT_Export"
#include <functional>
#include "spdlog/pattern_formatter.h"
export module VT.Event:Base;

export namespace VT
{
enum class EventType
{
    eWindowClose,
    eWindowResize,
    eWindowFocus,
    eWindowLostFocus,
    eWindowMoved,

    eAppTick,
    eAppUpdate,
    eAppRender,

    eKeyPress,
    eKeyRelease,

    eMouseButtonPress,
    eMouseButtonRelease,
    eMouseMove,
    eMouseScroll
};

#define BIT(pos) (1 << pos)
enum EventCategory
{
    EventCategoryNone        = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput       = BIT(1),
    EventCategoryKeyboard    = BIT(2),
    EventCategoryMouse       = BIT(3),
    EventCategoryMouseButton = BIT(4)
};

class VT_ENGINE_EXPORT Event
{
    friend class EventDispatcher;

    template <typename T>
    friend bool Dispatch(Event&, std::function<bool(T&)>);

public:
    virtual constexpr EventType GetEventType() const       = 0;
    virtual constexpr const char* GetName() const          = 0;
    virtual constexpr unsigned int GetCategoryFlag() const = 0;

    constexpr bool Handled() { return m_Handled; }

    constexpr bool IsInCategory(EventCategory Category) { return GetCategoryFlag() & Category; }

    virtual ~Event() {}

protected:
    bool m_Handled {false};
};

class EventDispatcher
{
public:
    EventDispatcher(Event& E) : m_Event(E) {}

    template <typename Type>
    // requires requires { std::is_base_of<Event, Type::GetType()>; }
    bool Dispatch(std::function<bool(Type&)> Function)
    {
        // check Event and the handling function match
        if (m_Event.GetEventType() == Type::GetType())
        {
            m_Event.m_Handled = Function(dynamic_cast<Type&>(m_Event));
            return true;
        }

        return false;
    }

    virtual ~EventDispatcher() {}

private:
    Event& m_Event;
};

template <typename Type>
// requires requires { std::is_base_of<Event, Type::GetType()>; }
bool Dispatch(Event& Event, std::function<bool(Type&)> Function)
{
    // check Event and the handling function match
    if (Event.GetEventType() == Type::GetType())
    {
        Event.m_Handled = Function(dynamic_cast<Type&>(Event));
    }

    return Event.m_Handled;
}
} // namespace VT

export template <>
struct std::formatter<VT::Event>
{
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const VT::Event& E, std::format_context& ctx) const
    {
        return std::format_to(ctx.out(), "{}", E.GetName());
    }
};
