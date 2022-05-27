#pragma once
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

#define _REF_TO_REF                                \
    {                                              \
        return object0.get()->operator==(object1); \
    }

template <typename T, typename R>
bool operator==(const ref<T> &object0, const ref<R> &object1) _REF_TO_REF;
template <typename T, typename R>
bool operator==(const ref<T> &object0, ref<R> &&object1) _REF_TO_REF;
template <typename T, typename R>
bool operator==(ref<T> &&object0, const ref<R> &object1) _REF_TO_REF;
template <typename T, typename R>
bool operator==(ref<T> &&object0, ref<R> &&object1) _REF_TO_REF;

template <typename T, typename R>
bool operator!=(const ref<T> &object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const ref<T> &object0, ref<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, ref<R> &&object1) _REVERSE;

#define _REF_TO_OPTION                                      \
    {                                                       \
        if (R *ptr = object1.get())                         \
        {                                                   \
            object0.get()->operator==(Object::cast<>(ptr)); \
        }                                                   \
        return false;                                       \
    }

template <typename T, typename R>
bool operator==(const ref<T> &object0, const option<R> &object1) _REF_TO_OPTION;
template <typename T, typename R>
bool operator==(const ref<T> &object0, option<R> &&object1) _REF_TO_OPTION;
template <typename T, typename R>
bool operator==(ref<T> &&object0, const option<R> &object1) _REF_TO_OPTION;
template <typename T, typename R>
bool operator==(ref<T> &&object0, option<R> &&object1) _REF_TO_OPTION;

template <typename T, typename R>
bool operator!=(const ref<T> &object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const ref<T> &object0, option<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(ref<T> &&object0, option<R> &&object1) _REVERSE;

#define _OPTION_TO_REF             \
    {                              \
        return object1 == object0; \
    }

template <typename T, typename R>
bool operator==(const option<T> &object0, const ref<R> &object1) _OPTION_TO_REF;
template <typename T, typename R>
bool operator==(const option<T> &object0, ref<R> &&object1) _OPTION_TO_REF;
template <typename T, typename R>
bool operator==(option<T> &&object0, const ref<R> &object1) _OPTION_TO_REF;
template <typename T, typename R>
bool operator==(option<T> &&object0, ref<R> &&object1) _OPTION_TO_REF;

template <typename T, typename R>
bool operator!=(const option<T> &object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const option<T> &object0, ref<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, const ref<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, ref<R> &&object1) _REVERSE;

#define _OPTION_TO_OPTION                                        \
    {                                                            \
        T *ptr0;                                                 \
        R *ptr1;                                                 \
        if ((ptr0 = object0.get()) && (ptr1 = object1.get()))    \
        {                                                        \
            return Object::cast<>(ptr0) == Object::cast<>(ptr1); \
        }                                                        \
        return false;                                            \
    }

template <typename T, typename R>
bool operator==(const option<T> &object0, const option<R> &object1) _OPTION_TO_OPTION;
template <typename T, typename R>
bool operator==(const option<T> &object0, option<R> &&object1) _OPTION_TO_OPTION;
template <typename T, typename R>
bool operator==(option<T> &&object0, const option<R> &object1) _OPTION_TO_OPTION;
template <typename T, typename R>
bool operator==(option<T> &&object0, option<R> &&object1) _OPTION_TO_OPTION;

template <typename T, typename R>
bool operator!=(const option<T> &object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(const option<T> &object0, option<R> &&object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, const option<R> &object1) _REVERSE;
template <typename T, typename R>
bool operator!=(option<T> &&object0, option<R> &&object1) _REVERSE;

template <typename T>
bool operator==(const option<T> &object0, std::nullptr_t) { return object0.get() == nullptr; }
template <typename T>
bool operator==(option<T> &&object0, std::nullptr_t) { return object0.get() == nullptr; }
template <typename T>
bool operator!=(const option<T> &object0, std::nullptr_t) { return object0.get() != nullptr; }
template <typename T>
bool operator!=(option<T> &&object0, std::nullptr_t) { return object0.get() != nullptr; }

/**
 * @brief Object
 *
 */

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &object)
{
    object->toStringStream(os);
    return os;
}
