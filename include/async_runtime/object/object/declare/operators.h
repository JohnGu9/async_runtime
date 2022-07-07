#pragma once
#include "basic.h"
#include <ostream>

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &);

template <typename T>
std::ostream &operator<<(std::ostream &os, ref<T> &&);

template <typename T>
std::ostream &operator<<(std::ostream &os, const option<T> &);

template <typename T>
std::ostream &operator<<(std::ostream &os, option<T> &&);

/**
 * @brief marco for option
 *
 * @example
 * option<Object> a;
 * if_not_null(a)
 * {
 *  // inside scope, [a] become ref<Object>
 *  // you can call its members directly like below
 *
 *  std::cout << a->toString() << std::endl;
 *
 * }
 * end_of(); // don't forget use [end_of()] to close the scope
 *
 * if_object_is(a, MyType)
 * {
 *  // inside scope, [a] become ref<MyType>
 *  // you can call its members directly like below
 *
 *  a->myFunction();
 * }
 * end_if(); // don't forget use [end_of()] to close the scope
 *
 */

/**
 * @brief [if_object_is] or [else_if_object_is].
 * check object type and cast it
 *
 * @param object ref / lateref / option
 * @param T a type that is derived of [Object]
 *
 */
#define if_object_is(object, T)                          \
    if (auto __ptr__ == dynamic_cast<T *>(object.get())) \
    {                                                    \
        auto object = ::Object::cast<>(__ptr__);
#define else_if_object_is(object, T)                          \
    }                                                         \
    else if (auto __ptr__ == dynamic_cast<T *>(object.get())) \
    {                                                         \
        auto object = ::Object::cast<>(__ptr__);

/**
 * @brief [if_not_null] or [else_if_not_null].
 * check object whether is null, if not null, convert it to ref
 *
 * @param object option
 *
 */
#define if_not_null(object)          \
    if (auto __ptr__ = object.get()) \
    {                                \
        auto object = ::Object::cast<>(__ptr__);
#define else_if_not_null(object)          \
    }                                     \
    else if (auto __ptr__ = object.get()) \
    {                                     \
        auto object = ::Object::cast<>(__ptr__);

#define else_if(statement) \
    }                      \
    else if ((statement))  \
    {
#define else_end() \
    }              \
    else           \
    {
#define end_if() \
    }
