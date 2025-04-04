#include "tools/log.hpp"

#if defined(BEE_PLATFORM_PC)

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

void bee::Log::Initialize()
{
    spdlog::cfg::load_env_levels();

#ifdef PLATFORM_PC
    auto console = spdlog::create<spdlog::sinks::msvc_sink_mt>("vs out");
    spdlog::set_default_logger(console);
    spdlog::set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");
#elif PLATFORM_SWITCH
    spdlog::set_pattern("[%^%l%$] %v");
#endif

    spdlog::info("Spdlog Version {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);

#if 0 // Samples from spdlog for testing
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:>8} aligned, {:<8} aligned", "right", "left");
    spdlog::error("ERR!?");
#endif

    spdlog::info("  ___   ___   ___ ");
    spdlog::info(" | _ ) | __| | __|");
    spdlog::info(" | _ \\ | _|  | _| ");
    spdlog::info(" |___/ |___| |___|");
    spdlog::info("                   ");
    spdlog::info("BUAS Example Engine");
}

#endif