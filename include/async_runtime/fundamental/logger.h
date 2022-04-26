#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <time.h>

#include "async.h"

class LoggerHandler : public virtual Object
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
