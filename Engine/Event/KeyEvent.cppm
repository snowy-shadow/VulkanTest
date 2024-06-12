module;
#include "VT_Export"
#include <cstdint>
#include "EngineMacro.h"

export module Event.Key;
import Event;

export namespace VT
{
class VT_ENGINE_EXPORT KeyEvent : public Event
{
public:
    constexpr uint32_t GetKeyCode() const { return m_KeyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(uint32_t KeyCode) : m_KeyCode(KeyCode) {}

    uint32_t m_KeyCode;
};

class VT_ENGINE_EXPORT KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(uint32_t KeyCode, uint32_t RepeatCount) : KeyEvent(KeyCode), m_RepeatCount(RepeatCount) {}

    constexpr uint32_t GetRepeatCount() const { return m_RepeatCount; }

    EVENT_CLASS_TYPE(EventType::eKeyPressed)

private:
    uint32_t m_RepeatCount;
};

class VT_ENGINE_EXPORT KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(uint32_t KeyCode) : KeyEvent(KeyCode) {}

    EVENT_CLASS_TYPE(EventType::eKeyReleased)
};
} // namespace VT
