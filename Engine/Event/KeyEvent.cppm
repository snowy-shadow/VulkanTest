module;
#include "VT_Export"
#include "EngineMacro.h"

export module Event:Key;
import :Base;

export namespace VT
{
class VT_ENGINE_EXPORT KeyEvent : public Event
{
public:
    constexpr unsigned int GetKeyCode() const { return m_KeyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(unsigned int KeyCode) : m_KeyCode(KeyCode) {}

    unsigned int m_KeyCode;
};

class VT_ENGINE_EXPORT KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(unsigned int KeyCode, unsigned int RepeatCount) : KeyEvent(KeyCode), m_RepeatCount(RepeatCount) {}

    constexpr unsigned int GetRepeatCount() const { return m_RepeatCount; }

    EVENT_CLASS_TYPE(EventType::eKeyPressed)

private:
    unsigned int m_RepeatCount;
};

class VT_ENGINE_EXPORT KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(unsigned int KeyCode) : KeyEvent(KeyCode) {}

    EVENT_CLASS_TYPE(EventType::eKeyReleased)
};
} // namespace VT
