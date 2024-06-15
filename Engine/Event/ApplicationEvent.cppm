module;

#include "VT_Export"
#include "EngineMacro.h"

#include <array>
#include <cstdint>
export module Event:Application;
import :Base;

export namespace VT
{
class VT_ENGINE_EXPORT WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(const std::array<uint32_t, 2>& DimensionXY) : m_DimensionXY(DimensionXY) {}
    constexpr std::array<uint32_t, 2> GetDimensionXY() { return m_DimensionXY; }

    EVENT_CLASS_TYPE(EventType::eWindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    const std::array<uint32_t, 2> m_DimensionXY;
};

class VT_ENGINE_EXPORT WindowCloseEvent : public Event
{
public:
    EVENT_CLASS_TYPE(EventType::eWindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class VT_ENGINE_EXPORT AppTickEvent : public Event
{
public:
    EVENT_CLASS_TYPE(EventType::eAppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class VT_ENGINE_EXPORT AppUpdateEvent : public Event
{
public:
    EVENT_CLASS_TYPE(EventType::eAppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class VT_ENGINE_EXPORT AppRenderEvent : public Event
{
public:
    EVENT_CLASS_TYPE(EventType::eAppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
} // namespace VT
