module;
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "VT_Export"
#include <memory>
export module VT.Log;
// import std;

/*
 * singleton logger
 */
export namespace VT
{
class Log
{
public:
    std::shared_ptr<spdlog::logger> CoreLogger;
    std::shared_ptr<spdlog::logger> ClientLogger;
    VT_ENGINE_EXPORT static std::shared_ptr<Log> Instance()
    {
        static std::shared_ptr<Log> Logger {new Log()};
        return Logger;
    }

private:
    Log() : CoreLogger {spdlog::stdout_color_mt("Engine")}, ClientLogger {spdlog::stdout_color_mt("App")}
    {
        spdlog::set_pattern("%^[%T] %n : %v%$");
        CoreLogger->set_level(spdlog::level::trace);
        ClientLogger->set_level(spdlog::level::trace);
    }
};
} // namespace VT
