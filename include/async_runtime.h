#pragma once

// export library
#include "async_runtime/widgets/widget.h"
#include "async_runtime/widgets/leaf_widget.h"
#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/widgets/multi_child_widget.h"
#include "async_runtime/widgets/named_multi_child_widget.h"
#include "async_runtime/widgets/inherited_widget.h"
#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/widgets/builder.h"
#include "async_runtime/widgets/value_listenable_builder.h"
#include "async_runtime/widgets/future_builder.h"

#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"

#ifdef ASYNC_RUNTIME_DISABLE_CONSOLE
#define __WITH_CONSOLE false
#else
#define __WITH_CONSOLE true
#endif

int runApp(ref<Widget> widget, bool withConsole = __WITH_CONSOLE);

#ifndef __FILENAME__
#ifndef NDEBUG
#define __FILENAME__ __FILE__
#else
namespace _async_runtime
{
    /**
     * @brief
     * reserve file name only that without full path
     *
     * @example
     * /usr/local/async_runtime_test/async_runtime/src/object.cpp
     * ->
     * object.cpp
     *
     */
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

#define _ASYNC_RUNTIME_LOG_FORMAT(_type, _format, ...)                                                                                                                                                 \
    {                                                                                                                                                                                                  \
        std::stringstream __ss__;                                                                                                                                                                      \
        struct tm __buf__;                                                                                                                                                                             \
        time_t __t__ = time(nullptr);                                                                                                                                                                  \
        ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__);                                                                                                                                                   \
        __ss__ << std::put_time(&__buf__, ASYNC_RUNTIME_TIMESTAMP_FORMAT) << " [" << __FILENAME__ << ":" << __LINE__ << "] [" << _type << "] " ASYNC_RUNTIME_OSTREAM_REDIRECT << _format << std::endl; \
        Logger::of(context)->write(ref<String>(__ss__.str())->format(__VA_ARGS__));                                                                                                                    \
    }

#ifndef NDEBUG
#define LogDebug(_format, ...) _ASYNC_RUNTIME_LOG_FORMAT("DEBUG", _format, __VA_ARGS__)
#else
#define LogDebug(_format, ...) ((void)0)
#endif
#define LogInfo(_format, ...) _ASYNC_RUNTIME_LOG_FORMAT("INFO ", _format, __VA_ARGS__)
#define LogWarning(_format, ...) _ASYNC_RUNTIME_LOG_FORMAT("WARNING", _format, __VA_ARGS__)
#define LogError(_format, ...) _ASYNC_RUNTIME_LOG_FORMAT("ERROR", _format, __VA_ARGS__)
