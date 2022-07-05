#pragma once
#include "cross_implement.h"
#include "object.h"
#include "option.h"
#include "ref.h"

/**
 * @brief operator<<
 *
 */

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &object)
{
    std::lock_guard<std::recursive_mutex> lk(_async_runtime::OstreamStack::mutex);
    ++_async_runtime::OstreamStack::depth;
    object->toStringStream(os);
    --_async_runtime::OstreamStack::depth;
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, ref<T> &&object)
{
    std::lock_guard<std::recursive_mutex> lk(_async_runtime::OstreamStack::mutex);
    ++_async_runtime::OstreamStack::depth;
    object->toStringStream(os);
    --_async_runtime::OstreamStack::depth;
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const option<T> &object)
{
    object.ifNotNull([&](ref<T> object) { //
              os << object;
          })
        .ifElse([&] { //
            os << "[NULL]";
        });
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, option<T> &&object)
{
    object.ifNotNull([&](ref<T> object) { //
              os << object;
          })
        .ifElse([&] { //
            os << "[NULL]";
        });
    return os;
}
