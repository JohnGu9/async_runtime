#pragma once
#include "option.h"
#include "ref.h"

template <typename T>
bool operator==(const option<T> &opt, std::nullptr_t) { return static_cast<const std::shared_ptr<T> &>(opt) == nullptr; }
template <typename T>
bool operator!=(const option<T> &opt, std::nullptr_t) { return !(opt == nullptr); }

template <typename T, typename R>
bool operator==(const option<T> &object0, option<R> object1) { return static_cast<const std::shared_ptr<T> &>(object0) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T, typename R>
bool operator!=(const option<T> &object0, option<R> object1) { return !(object0 == object1); }

template <typename T, typename R>
bool operator==(const ref<T> &object0, ref<R> object1) { return static_cast<const std::shared_ptr<T> &>(object0) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T, typename R>
bool operator!=(const ref<T> &object0, ref<R> object1) { return !(object0 == object1); }

template <typename T, typename R>
bool operator==(const ref<T> &object0, option<R> object1) { return static_cast<const std::shared_ptr<T> &>(object0) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T, typename R>
bool operator!=(const ref<T> &object0, option<R> object1) { return !(object0 == object1); }

template <typename T, typename R>
bool operator==(const option<T> &object0, ref<R> object1) { return static_cast<const std::shared_ptr<T> &>(object0) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T, typename R>
bool operator!=(const option<T> &object0, ref<R> object1) { return !(object0 == object1); }

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &object)
{
    object->toStringStream(os);
    return os;
}
