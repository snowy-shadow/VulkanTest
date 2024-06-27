module;
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "VT_Export"
#include <memory>
export module VT.Log;
import VT.Util;
// import std;

/*
 * singleton logger
 */
export namespace VT
{
class Log
{
public:
    Shared<spdlog::logger> CoreLogger;
    Shared<spdlog::logger> ClientLogger;
    VT_ENGINE_EXPORT static Shared<Log> Instance()
    {
        static Shared<Log> Logger {new Log()};
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
