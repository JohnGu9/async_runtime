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
 * @usage
 *
 * option<Object> a;
 * if_not_null(a)
 * {
 *  // inside scope, [a] become ref<Object>
 *  // you can call its members directly like below
 *
 *  std::cout << a->toString() << std::endl;
 *
 * }
 * end_of() // don't forget use [end_of()] to close the scope
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
