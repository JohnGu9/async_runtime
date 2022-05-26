#pragma once
#include "basic.h"
#include <iostream>
#include <sstream>

template <typename T>
bool operator==(const option<T> &opt, std::nullptr_t);
template <typename T>
bool operator!=(const option<T> &opt, std::nullptr_t);

template <typename T, typename R>
bool operator==(const option<T> &object0, option<R> object1);
template <typename T, typename R>
bool operator!=(const option<T> &object0, option<R> object1);

template <typename T, typename R>
bool operator==(const ref<T> &object0, ref<R> object1);
template <typename T, typename R>
bool operator!=(const ref<T> &object0, ref<R> object1);

template <typename T, typename R>
bool operator==(const ref<T> &object0, option<R> object1);
template <typename T, typename R>
bool operator!=(const ref<T> &object0, option<R> object1);

template <typename T, typename R>
bool operator==(const option<T> &object0, ref<R> object1);
template <typename T, typename R>
bool operator!=(const option<T> &object0, ref<R> object1);

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &);
