#pragma once

#include "../option.h"

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
option<T>::option(const ref<R> &other) noexcept : super(other) {}

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
option<T>::option(ref<R> &&other) noexcept : super(std::move(other)) {}

template <typename T>
template <typename First, typename... Args>
option<T>::option(const First &first, Args &&...args) : super(ref<T>(first, std::forward<Args>(args)...)) {}
