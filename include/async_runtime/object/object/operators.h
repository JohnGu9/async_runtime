#pragma once
#include "cross_implement.h"
#include "object.h"
#include "option.h"
#include "ref.h"

/**
 * @brief cast
 *
 */

#define _REVERSE                      \
    {                                 \
        return !(object0 == object1); \
    }

template <typename T, typename R>
bool operator==(const ref<T> &object0, const ref<R> &object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(const ref<T> &object0, ref<R> &&object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(ref<T> &&object0, const ref<R> &object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(ref<T> &&object0, ref<R> &&object1) { return Object::equal<>(object0, object1); }

template <typename T, typename R>
bool operator!=(const ref<T> &object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const ref<T> &object0, ref<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, ref<R> &&object1) _REVERSE;

template <typename T, typename R>
bool operator==(const ref<T> &object0, const option<R> &object1)
{
    return Object::equal<>(object0, object1);
}
template <typename T, typename R>
bool operator==(const ref<T> &object0, option<R> &&object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(ref<T> &&object0, const option<R> &object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(ref<T> &&object0, option<R> &&object1) { return Object::equal<>(object0, object1); }

template <typename T, typename R>
bool operator!=(const ref<T> &object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const ref<T> &object0, option<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, option<R> &&object1) _REVERSE;

template <typename T, typename R>
bool operator==(const option<T> &object0, const ref<R> &object1)
{
    return Object::equal<>(object0, object1);
}
template <typename T, typename R>
bool operator==(const option<T> &object0, ref<R> &&object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(option<T> &&object0, const ref<R> &object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(option<T> &&object0, ref<R> &&object1) { return Object::equal<>(object0, object1); }

template <typename T, typename R>
bool operator!=(const option<T> &object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const option<T> &object0, ref<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, ref<R> &&object1) _REVERSE;

template <typename T, typename R>
bool operator==(const option<T> &object0, const option<R> &object1)
{
    return Object::equal<>(object0, object1);
}
template <typename T, typename R>
bool operator==(const option<T> &object0, option<R> &&object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(option<T> &&object0, const option<R> &object1) { return Object::equal<>(object0, object1); }
template <typename T, typename R>
bool operator==(option<T> &&object0, option<R> &&object1) { return Object::equal<>(object0, object1); }

template <typename T, typename R>
bool operator!=(const option<T> &object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const option<T> &object0, option<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, option<R> &&object1) _REVERSE;

template <typename T>
bool operator==(const option<T> &object0, std::nullptr_t) { return Object::isNull<>(object0); }
template <typename T>
bool operator==(option<T> &&object0, std::nullptr_t) { return Object::isNull<>(object0); }
template <typename T>
bool operator!=(const option<T> &object0, std::nullptr_t) { return !Object::isNull<>(object0); }
template <typename T>
bool operator!=(option<T> &&object0, std::nullptr_t) { return !Object::isNull<>(object0); }

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