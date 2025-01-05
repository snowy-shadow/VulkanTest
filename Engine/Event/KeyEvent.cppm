module;
#include "VT_Export"
#include "EventMacro.h"

export module VT.Event:Key;
import :Base;
export import VT.Key;

export namespace VT
{

class VT_ENGINE_EXPORT KeyEvent : public Event
{
public:
    constexpr Key GetKeyCode() const { return m_Key; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(Key Key) : m_Key(Key) {}

    Key m_Key;
};

class VT_ENGINE_EXPORT KeyPressEvent : public KeyEvent
{
public:
    KeyPressEvent(Key Key, unsigned int RepeatCount = 0) : KeyEvent(Key), m_RepeatCount(RepeatCount) {}

    constexpr unsigned int GetRepeatCount() const { return m_RepeatCount; }

    EVENT_CLASS_TYPE(EventType::eKeyPress)

private:
    unsigned int m_RepeatCount;
};

class VT_ENGINE_EXPORT KeyReleaseEvent : public KeyEvent
{
public:
    KeyReleaseEvent(Key Key) : KeyEvent(Key) {}

    EVENT_CLASS_TYPE(EventType::eKeyRelease)
};
} // namespace VT
