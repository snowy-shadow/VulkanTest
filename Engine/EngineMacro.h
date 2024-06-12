#pragma once

/* ==========================================
 *               LOGGER
 * ==========================================
 */
#ifdef NDBUG // Release mode
    #define VT_CORE_FATAL(...)
    #define VT_CORE_ERROR(...)
    #define VT_CORE_WARN(...)
    #define VT_CORE_INFO(...)
    #define VT_CORE_TRACE(...)

    #define VT_FATAL(...)
    #define VT_ERROR(...)
    #define VT_WARN(...)
    #define VT_INFO(...)
    #define VT_TRACE(...)
#else
    #define VT_CORE_FATAL(...) VT::Log::Instance()->CoreLogger->fatal(__VA_ARGS__);
    #define VT_CORE_ERROR(...) VT::Log::Instance()->CoreLogger->error(__VA_ARGS__);
    #define VT_CORE_WARN(...)  VT::Log::Instance()->CoreLogger->warn(__VA_ARGS__);
    #define VT_CORE_INFO(...)  VT::Log::Instance()->CoreLogger->info(__VA_ARGS__);
    #define VT_CORE_TRACE(...) VT::Log::Instance()->CoreLogger->trace(__VA_ARGS__);

    #define VT_FATAL(...) VT::Log::Instance()->ClientLogger->fatal(__VA_ARGS__);
    #define VT_ERROR(...) VT::Log::Instance()->ClientLogger->error(__VA_ARGS__);
    #define VT_WARN(...)  VT::Log::Instance()->ClientLogger->warn(__VA_ARGS__);
    #define VT_INFO(...)  VT::Log::Instance()->ClientLogger->info(__VA_ARGS__);
    #define VT_TRACE(...) VT::Log::Instance()->ClientLogger->trace(__VA_ARGS__);
#endif

/* ==========================================
 *               Event Class Helper
 * ==========================================
 */
#define EVENT_CLASS_TYPE(Type)                                              \
    static constexpr EventType GetType() { return Type; }                   \
    constexpr EventType GetEventType() const override { return GetType(); } \
    constexpr const char* GetName() const override { return #Type; }

#define EVENT_CLASS_CATEGORY(Category) \
    constexpr uint32_t GetCategoryFlag() const override { return Category; }
