module;
#include <array>

#include "VT_Export"
#include "EngineMacro.h"

export module VT.Event:Mouse;
import :Base;

export namespace VT
{
class VT_ENGINE_EXPORT MouseMoveEvent : public Event
{
public:
    MouseMoveEvent(const std::array<float, 2>& XY) : m_XY(XY) {}
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
    const std::array<float, 2> m_OffsetXY;
};

class VT_ENGINE_EXPORT MouseButtonEvent : public Event
{
public:
    constexpr int GetButton() const { return m_Button; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

protected:
    MouseButtonEvent(int Button) : m_Button {Button} {}

    int m_Button;
};

class VT_ENGINE_EXPORT MouseButtonPressEvent : public MouseButtonEvent
{
public:
    MouseButtonPressEvent(int Button) : MouseButtonEvent(Button) {}

    EVENT_CLASS_TYPE(EventType::eMouseButtonPress)
};

class VT_ENGINE_EXPORT MouseButtonReleaseEvent : public MouseButtonEvent
{

public:
    MouseButtonReleaseEvent(int Button) : MouseButtonEvent(Button) {}

    EVENT_CLASS_TYPE(EventType::eMouseButtonRelease)
};

} // namespace VT
