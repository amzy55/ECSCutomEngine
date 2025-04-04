#pragma once

#if defined(BEE_PLATFORM_PC)
#if defined(APIENTRY)
#undef APIENTRY
#endif
#include <spdlog/spdlog.h>
#endif

namespace bee
{

/// <summary>
/// Logger class, using spdlog.
/// </summary>
class Log
{
public:
    /// <summary>
    /// Initialize the logger, setting the pattern and the level of logging.
    /// </summary>
    static void Initialize();

    /// <summary>
    /// Info level logging.
    /// </summary>
    /// <param name="fmt">Format string, using Python-like format string syntax.</param>
    ///	<param name="...args">List of positional arguments.</param>
    template <typename FormatString, typename... Args>
    static void Info(const FormatString& fmt, const Args&... args);

    /// <summary>
    /// Warning level logging. Engine can still run as intended.
    /// </summary>
    /// <param name="fmt">Format string, using Python-like format string syntax.</param>
    ///	<param name="...args">List of positional arguments.</param>
    template <typename FormatString, typename... Args>
    static void Warn(const FormatString& fmt, const Args&... args);

    /// <summary>
    /// Error level logging. Engine may still be able to run, but not as intended.
    /// </summary>
    /// <param name="fmt">Format string, using Python-like format string syntax.</param>
    /// <param name="...args">List of positional arguments.</param>
    template <typename FormatString, typename... Args>
    static void Error(const FormatString& fmt, const Args&... args);

    /// <summary>
    /// Critical level logging. Engine cannot run.
    /// </summary>
    /// <param name="fmt">Format string, using Python-like format string syntax.</param>
    ///	<param name="...args">List of positional arguments.</param>
    template <typename FormatString, typename... Args>
    static void Critical(const FormatString& fmt, const Args&... args);

private:
    static constexpr auto magenta = "\033[35m";
    static constexpr auto green = "\033[32m";
    static constexpr auto red = "\033[31m";
    static constexpr auto reset = "\033[0m";
};

#ifdef BEE_PLATFORM_PC

template <typename FormatString, typename... Args>
inline void Log::Info(const FormatString& fmt, const Args&... args)
{
    spdlog::info(fmt, args...);
}

template <typename FormatString, typename... Args>
inline void Log::Warn(const FormatString& fmt, const Args&... args)
{
    spdlog::warn(fmt, args...);
}

template <typename FormatString, typename... Args>
inline void Log::Error(const FormatString& fmt, const Args&... args)
{
    spdlog::error(fmt, args...);
}

template <typename FormatString, typename... Args>
inline void Log::Critical(const FormatString& fmt, const Args&... args)
{
    spdlog::critical(fmt, args...);
}

#endif

} // namespace bee