/**
 * @brief
 *
 * @example
 * // Create custom logger [MyLoggerHandler:
 * class MyLoggerHandler : public LoggerHandler
 * {
 * public:
 *      ref<Future<bool>> write(ref<String> str) override { / **** / }
 * }
 *
 *
 *  // Wrap your widget with [Logger]
 *  Object::create<Logger>(
 *      [YourWidget], // your widget to wrap
 *      Object::create<MyLoggerHandler>(), // put your handler into [Logger]
 *  );
 *
 *
 *  // Get [MyLoggerHandler] from [BuildContext]
 *  ref<Widget> build(ref<BuildContext> context) override
 *  {
 *      ref<LoggerHandler> myLoggerHandler = Logger::of(context);
 *      myLoggerHandler->writeLine("get MyLoggerHandler");
 *
 *      / **** /
 *
 *      return ${yourWidget};
 *  }
 *
 *  Use Logger macros [LogDebug, LogInfo, LogWarning, LogError] (define in file async_runtime.h)
 *  macro support two format [printf style] [istream style]
 *  [printf style]: LogDebug("Omg {}! {}.", "args", 26); // the "{}" will be filled with arguments
 *
 *  both output same stuff
 *
 */

#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <time.h>

#include "async.h"

class LoggerHandler : public Object
{
public:
    LoggerHandler() {}

    virtual ref<Future<bool>> write(ref<String> str) = 0;
    virtual ref<Future<bool>> writeLine(ref<String> str) = 0;
    virtual void dispose() = 0;
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
