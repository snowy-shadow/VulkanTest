module;
#include <cstdint>
#include <array>

#include "VT_Export"
#include "EngineMacro.h"
export module Event.Mouse;
import Event;

namespace VT
{
class VT_ENGINE_EXPORT MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(const std::array<float, 2>& XY) : m_XY(XY) {}

    constexpr std::array<float, 2> GetXY() { return m_XY; }

    EVENT_CLASS_TYPE(EventType::eMouseMove)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    const std::array<float, 2> m_XY;
};

class VT_ENGINE_EXPORT MouseScrollEvent : public Event
{
public:
    MouseScrollEvent(const std::array<float, 2>& OffsetXY) : m_OffsetXY(OffsetXY) {}

    EVENT_CLASS_TYPE(EventType::eMouseScroll)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    const std::array<float, 2> m_OffsetXY[2];
};

class VT_ENGINE_EXPORT MouseButtonEvent : public Event
{
public:
    constexpr uint32_t GetButton() const { return m_Button; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

protected:
    MouseButtonEvent(uint32_t Button) : m_Button {Button} {}

    uint32_t m_Button;
};

class VT_ENGINE_EXPORT MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(uint32_t Button) : MouseButtonEvent(Button) {}

    EVENT_CLASS_TYPE(EventType::eMouseButtonPressed)
};

class VT_ENGINE_EXPORT MouseButtonReleasedEvent : public MouseButtonEvent
{

public:
    MouseButtonReleasedEvent(uint32_t Button) : MouseButtonEvent(Button) {}

    EVENT_CLASS_TYPE(EventType::eMouseButtonReleased)
};

} // namespace VT
