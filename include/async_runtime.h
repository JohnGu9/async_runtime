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
#include "async_runtime/widgets/global_key.h"
#include "async_runtime/widgets/inherited_widget.h"
#include "async_runtime/widgets/key.h"
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
     * clip file name from file's full-path
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

// for std::put_time
#include <iomanip>

namespace _async_runtime
{
    extern const char *debug_logger_type;
    extern const char *info_logger_type;
    extern const char *warning_logger_type;
    extern const char *error_logger_type;

    inline std::stringstream &loggerTime(std::stringstream &ss)
    {
        struct tm timebuf;
        time_t t = time(nullptr);
#ifdef _WIN32
        localtime_s(&timebuf, &t);
#else
        localtime_r(&t, &timebuf);
#endif
        ss << std::put_time(&timebuf, ASYNC_RUNTIME_TIMESTAMP_FORMAT);

#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
        ss << std::boolalpha;
#endif

        return ss;
    }
}

#ifndef ASYNC_RUNTIME_OSTREAM_REDIRECT
#define ASYNC_RUNTIME_OSTREAM_REDIRECT
#endif

#ifndef ASYNC_RUNTIME_LOG_FORMAT
#define ASYNC_RUNTIME_LOG_FORMAT(__type__, __format__, ...)                                                                                    \
    {                                                                                                                                          \
        ::std::stringstream __stringstream__("");                                                                                              \
        ::_async_runtime::loggerTime(__stringstream__)                                                                                         \
            << " [" << __FILENAME__ << ":" << __LINE__ << "] [" << __type__ << "] " ASYNC_RUNTIME_OSTREAM_REDIRECT << __format__ << std::endl; \
        ::Logger::of(context)                                                                                                                  \
            ->write(::Object::create<::String>(__stringstream__.str())->template format<>(__VA_ARGS__));                                       \
    }
#endif

#ifndef LogDebug
#ifndef NDEBUG
#define LogDebug(__format__, ...) ASYNC_RUNTIME_LOG_FORMAT(::_async_runtime::debug_logger_type, __format__, ##__VA_ARGS__)
#else
#define LogDebug(__format__, ...) ((void)0)
#endif
#endif

#ifndef LogInfo
#define LogInfo(__format__, ...) ASYNC_RUNTIME_LOG_FORMAT(::_async_runtime::info_logger_type, __format__, ##__VA_ARGS__)
#endif

#ifndef LogWarning
#define LogWarning(__format__, ...) ASYNC_RUNTIME_LOG_FORMAT(::_async_runtime::warning_logger_type, __format__, ##__VA_ARGS__)
#endif

#ifndef LogError
#define LogError(__format__, ...) ASYNC_RUNTIME_LOG_FORMAT(_async_runtime::error_logger_type, __format__, ##__VA_ARGS__)
#endif

#endif // ASYNC_RUNTIME_NO_EXPORT_WIDGETS
