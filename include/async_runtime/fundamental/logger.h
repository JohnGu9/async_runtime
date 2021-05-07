/// Create custom logger [MyLoggerHandler] example:
///
/// class MyLoggerHandler : public LoggerHandler
/// {
/// public:
///     ref<Future<bool>> write(ref<String> str) override { / **** / }
///     ref<Future<bool>> writeLine(ref<String> str) override { / **** / }
/// }
///
///
/// Wrap your widget with [Logger]
///
/// Object::create<Logger>(
///     [YourWidget], // your widget to wrap
///     Object::create<MyLoggerHandler>(), // put your handler into [Logger]
/// );
///
///
/// Get [MyLoggerHandler] from [BuildContext]
///
/// ref<Widget> build(ref<BuildContext> context) override
/// {
///     ref<LoggerHandler> myLoggerHandler = Logger::of(context);
///     myLoggerHandler->writeLine("get MyLoggerHandler");
///
///     / **** /
///
///     return ${yourWidget};
/// }
///
///
/// Use Logger macros [LogDebug, LogInfo, LogWarning, LogError]
/// macro support two format [printf style] [istream style]
/// [printf style]: LogDebug("Omg {}! {}.", "args", 26); // the "{}" will be filled with arguments
/// [istream style]: LogDebug("Omg "<< "args" << "! " << 26 << ". ");
///
/// both output same stuff
///

#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <time.h>

#include "../widgets/inherited_widget.h"
#include "../widgets/stateful_widget.h"
#include "state_helper.h"
#include "async.h"
#include "disposable.h"

#ifndef __FILENAME__
#ifndef NDEBUG
#define __FILENAME__ __FILE__
#else
namespace _async_runtime
{
    template <typename T, size_t S>
    inline constexpr size_t get_file_name_offset(const T (&str)[S], size_t i = S - 1)
    {
        return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
    }

    template <typename T>
    inline constexpr size_t get_file_name_offset(T (&str)[1])
    {
        return 0;
    }
};
#define __FILENAME__ &__FILE__[_async_runtime::get_file_name_offset(__FILE__)]
#endif
#endif

#define __STDC_WANT_LIB_EXT1__ 1

#ifndef ASYNC_RUNTIME_TIMESTAMP_FORMAT
// AsyncRuntime use std::put_time to output timestamp
// define ASYNC_RUNTIME_TIMESTAMP_FORMAT to custom your timestamp format
// https://en.cppreference.com/w/cpp/io/manip/put_time
#define ASYNC_RUNTIME_TIMESTAMP_FORMAT "%F %T"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__) localtime_s(&__buf__, &__t__)
#else
#define ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__) localtime_r(&__t__, &__buf__)
#endif

#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
#define ASYNC_RUNTIME_OSTREAM_REDIRECT << std::boolalpha
#else
#define ASYNC_RUNTIME_OSTREAM_REDIRECT
#endif

#ifndef NDEBUG
#define LogDebug(_format, ...)                                                                                                                                                       \
    {                                                                                                                                                                                \
        std::stringstream __ss__;                                                                                                                                                        \
        struct tm __buf__;                                                                                                                                                               \
        time_t __t__ = time(nullptr);                                                                                                                                                    \
        ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__);                                                                                                                                         \
        __ss__ << std::put_time(&__buf__, ASYNC_RUNTIME_TIMESTAMP_FORMAT) << " [" << __FILENAME__ << ":" << __LINE__ << "] [DEBUG] " ASYNC_RUNTIME_OSTREAM_REDIRECT << _format << std::endl; \
        Logger::of(context)->write(ref<String>(__ss__.str())->format(__VA_ARGS__));                                                                                                      \
    }
#else
#define LogDebug(_format, ...) \
    {                          \
    }
#endif

#define LogInfo(_format, ...)                                                                                                                                                       \
    {                                                                                                                                                                               \
        std::stringstream __ss__;                                                                                                                                                       \
        struct tm __buf__;                                                                                                                                                              \
        time_t __t__ = time(nullptr);                                                                                                                                                   \
        ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__);                                                                                                                                        \
        __ss__ << std::put_time(&__buf__, ASYNC_RUNTIME_TIMESTAMP_FORMAT) << " [" << __FILENAME__ << ":" << __LINE__ << "] [INFO] " ASYNC_RUNTIME_OSTREAM_REDIRECT << _format << std::endl; \
        Logger::of(context)->write(ref<String>(__ss__.str())->format(__VA_ARGS__));                                                                                                     \
    }

#define LogWarning(_format, ...)                                                                                                                                                       \
    {                                                                                                                                                                                  \
        std::stringstream __ss__;                                                                                                                                                          \
        struct tm __buf__;                                                                                                                                                                 \
        time_t __t__ = time(nullptr);                                                                                                                                                      \
        ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__);                                                                                                                                           \
        __ss__ << std::put_time(&__buf__, ASYNC_RUNTIME_TIMESTAMP_FORMAT) << " [" << __FILENAME__ << ":" << __LINE__ << "] [WARNING] " ASYNC_RUNTIME_OSTREAM_REDIRECT << _format << std::endl; \
        Logger::of(context)->write(ref<String>(__ss__.str())->format(__VA_ARGS__));                                                                                                        \
    }

#define LogError(_format, ...)                                                                                                                                                       \
    {                                                                                                                                                                                \
        std::stringstream __ss__;                                                                                                                                                        \
        struct tm __buf__;                                                                                                                                                               \
        time_t __t__ = time(nullptr);                                                                                                                                                    \
        ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__);                                                                                                                                         \
        __ss__ << std::put_time(&__buf__, ASYNC_RUNTIME_TIMESTAMP_FORMAT) << " [" << __FILENAME__ << ":" << __LINE__ << "] [ERROR] " ASYNC_RUNTIME_OSTREAM_REDIRECT << _format << std::endl; \
        Logger::of(context)->write(ref<String>(__ss__.str())->format(__VA_ARGS__));                                                                                                      \
    }

class LoggerHandler : public virtual Object, public Disposable
{
public:
    LoggerHandler() {}

    virtual ref<Future<bool>> write(ref<String> str) = 0;
    virtual ref<Future<bool>> writeLine(ref<String> str) = 0;
};

template <typename T>
const ref<LoggerHandler> &operator<<(const ref<LoggerHandler> &handle, const T &element)
{
    handle->write(std::to_string(element));
    return handle;
}

template <>
inline const ref<LoggerHandler> &operator<<(const ref<LoggerHandler> &handle, const ref<String> &element)
{
    handle->write(element);
    return handle;
}

template <typename T>
const ref<LoggerHandler> &operator<<(const ref<LoggerHandler> &handle, T &&element)
{
    handle->write(std::to_string(std::move(element)));
    return handle;
}

template <>
inline const ref<LoggerHandler> &operator<<(const ref<LoggerHandler> &handle, ref<String> &&element)
{
    handle->write(std::move(element));
    return handle;
}

class Logger : public InheritedWidget
{
public:
    using Handler = ref<LoggerHandler>;
    static Handler of(ref<BuildContext> context);

    // support hot switch
    static ref<Widget> cout(ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> file(ref<String> path, ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> block(ref<Widget> child, option<Key> key = nullptr);

    Logger(ref<Widget> child, Handler handler, option<Key> key = nullptr);
    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;

protected:
    Handler _handler;
};

class StdoutLogger : public StatefulWidget
{
public:
    static Logger::Handler of(ref<BuildContext> context);
    StdoutLogger(ref<Widget> child, option<Key> key = nullptr);
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
};

class StdoutLoggerState : public State<StdoutLogger>
{
public:
    using super = State<StdoutLogger>;
    lateref<LoggerHandler> _handler;

    void initState() override;
    void dispose() override;
    ref<Widget> build(ref<BuildContext>) override;
};

inline ref<State<StatefulWidget>> StdoutLogger::createState()
{
    return Object::create<StdoutLoggerState>();
}
