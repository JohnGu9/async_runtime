#pragma once

// export library
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/thread.h"
#include "async_runtime/fundamental/thread_pool.h"
#include "async_runtime/fundamental/timer.h"

// maybe you like to include widgets by your own
#ifndef ASYNC_RUNTIME_NO_EXPORT_WIDGETS

#include "async_runtime/widgets/builder.h"
#include "async_runtime/widgets/future_builder.h"
#include "async_runtime/widgets/inherited_widget.h"
#include "async_runtime/widgets/leaf_widget.h"
#include "async_runtime/widgets/logger_widget.h"
#include "async_runtime/widgets/multi_child_widget.h"
#include "async_runtime/widgets/named_multi_child_widget.h"
#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/widgets/root_widget.h"
#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/widgets/stream_builder.h"
#include "async_runtime/widgets/value_listenable_builder.h"
#include "async_runtime/widgets/widget.h"

// run widgets from this api
void runApp(ref<Widget> widget);

// ease logger marco
#ifndef __FILENAME__

#ifndef NDEBUG
/**
 * @brief
 * In debug mode, show the files' full-path
 *
 */
#define __FILENAME__ __FILE__
#else
namespace _async_runtime
{
    /**
     * @brief
     * reserve file name only without file's full-path
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
/**
 * @brief
 * In non-debug mode, only show the file name
 *
 */
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

#ifndef ASYNC_RUNTIME_BUILD_TIMEBUF
#ifdef _WIN32
#define ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__) localtime_s(&__buf__, &__t__)
#else
#define ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__) localtime_r(&__t__, &__buf__)
#endif
#endif

#ifndef ASYNC_RUNTIME_OSTREAM_REDIRECT
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
#define ASYNC_RUNTIME_OSTREAM_REDIRECT << std::boolalpha
#else
#define ASYNC_RUNTIME_OSTREAM_REDIRECT
#endif
#endif

#include <iomanip>

#ifndef _ASYNC_RUNTIME_LOG_FORMAT
#define _ASYNC_RUNTIME_LOG_FORMAT(__type__, __format__, ...)                                                                                                                                                 \
    {                                                                                                                                                                                                        \
        std::stringstream __ss__;                                                                                                                                                                            \
        struct tm __buf__;                                                                                                                                                                                   \
        time_t __t__ = time(nullptr);                                                                                                                                                                        \
        ASYNC_RUNTIME_BUILD_TIMEBUF(__t__, __buf__);                                                                                                                                                         \
        __ss__ << std::put_time(&__buf__, ASYNC_RUNTIME_TIMESTAMP_FORMAT) << " [" << __FILENAME__ << ":" << __LINE__ << "] [" << __type__ << "] " ASYNC_RUNTIME_OSTREAM_REDIRECT << __format__ << std::endl; \
        Logger::of(context)->write(ref<String>(__ss__.str())->format(__VA_ARGS__));                                                                                                                          \
    }
#endif

#ifndef LogDebug
#ifndef NDEBUG
#define LogDebug(__format__, ...) _ASYNC_RUNTIME_LOG_FORMAT("DEBUG", __format__, __VA_ARGS__)
#else
#define LogDebug(__format__, ...) ((void)0)
#endif
#endif

#ifndef LogInfo
#define LogInfo(__format__, ...) _ASYNC_RUNTIME_LOG_FORMAT("INFO ", __format__, __VA_ARGS__)
#endif

#ifndef LogWarning
#define LogWarning(__format__, ...) _ASYNC_RUNTIME_LOG_FORMAT("WARNING", __format__, __VA_ARGS__)
#endif

#ifndef LogError
#define LogError(__format__, ...) _ASYNC_RUNTIME_LOG_FORMAT("ERROR", __format__, __VA_ARGS__)
#endif

#endif
