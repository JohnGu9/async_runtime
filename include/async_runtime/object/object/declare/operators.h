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

#define if_not_null(object)          \
    if (auto __ptr__ = object.get()) \
    {                                \
        auto object = ::Object::cast<>(__ptr__);
#define else_if_not_null(object)          \
    }                                     \
    else if (auto __ptr__ = object.get()) \
    {                                     \
        auto object = ::Object::cast<>(__ptr__);
#define else_if(...)        \
    }                       \
    else if ((__VA_ARGS__)) \
    {
#define else_end() \
    }              \
    else           \
    {
#define end_if() };
