module;
#include "VT_Export"
#include "EngineMacro.h"

export module VT.Event:Key;
import :Base;

export namespace VT
{
class VT_ENGINE_EXPORT KeyEvent : public Event
{
public:
    constexpr int GetKeyCode() const { return m_KeyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(int KeyCode) : m_KeyCode(KeyCode) {}

    int m_KeyCode;
};

class VT_ENGINE_EXPORT KeyPressEvent : public KeyEvent
{
public:
    KeyPressEvent(int KeyCode, unsigned int RepeatCount = 0) : KeyEvent(KeyCode), m_RepeatCount(RepeatCount) {}

    constexpr unsigned int GetRepeatCount() const { return m_RepeatCount; }

    EVENT_CLASS_TYPE(EventType::eKeyPress)

private:
    unsigned int m_RepeatCount;
};

class VT_ENGINE_EXPORT KeyReleaseEvent : public KeyEvent
{
public:
    KeyReleaseEvent(int KeyCode) : KeyEvent(KeyCode) {}

    EVENT_CLASS_TYPE(EventType::eKeyRelease)
};
} // namespace VT
