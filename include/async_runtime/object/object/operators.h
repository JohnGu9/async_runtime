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
    std::lock_guard<std::recursive_mutex> lk(Object::OstreamCallStack::mutex);
    ++Object::OstreamCallStack::depth;
    object->toStringStream(os);
    --Object::OstreamCallStack::depth;
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, ref<T> &&object)
{
    std::lock_guard<std::recursive_mutex> lk(Object::OstreamCallStack::mutex);
    ++Object::OstreamCallStack::depth;
    object->toStringStream(os);
    --Object::OstreamCallStack::depth;
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const option<T> &object)
{
    if_not_null(object) return os << object;
    end_if() return os << typeid(T).name() << "[NULL]";
}

template <typename T>
std::ostream &operator<<(std::ostream &os, option<T> &&object)
{
    if_not_null(object) return os << object;
    end_if() return os << typeid(T).name() << "[NULL]";
}
