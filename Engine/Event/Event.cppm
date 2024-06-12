module;

#include <ostream>
#include <functional>
#include <cstdint>

#include "VT_Export"
export module Event;

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

    eKeyPressed,
    eKeyReleased,

    eMouseButtonPressed,
    eMouseButtonReleased,
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

public:
    virtual constexpr EventType GetEventType() const   = 0;
    virtual constexpr const char* GetName() const      = 0;
    virtual constexpr uint32_t GetCategoryFlag() const = 0;

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
            m_Event.m_Handled = Function(*(Type*) &m_Event);
            return true;
        }

        return false;
    }

    virtual ~EventDispatcher() {}

private:
    Event& m_Event;
};

} // namespace VT

export inline std::ostream& operator<<(std::ostream& OS, const VT::Event& E) { return OS << E.GetName(); }
