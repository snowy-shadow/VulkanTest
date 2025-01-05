#pragma once

/* ==========================================
 *              Event Class Helper
 * ==========================================
 */
#define EVENT_CLASS_TYPE(Type)                                              \
    static constexpr EventType GetType() { return Type; }                   \
    constexpr EventType GetEventType() const override { return GetType(); } \
    constexpr const char* GetName() const override { return #Type; }

#define EVENT_CLASS_CATEGORY(Category) \
    constexpr unsigned int GetCategoryFlag() const override { return Category; }