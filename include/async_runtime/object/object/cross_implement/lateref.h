#pragma once
#include "../lateref.h"

template <typename T>
template <typename First, typename... Args>
lateref<T>::lateref(const First &first, Args &&...args) : super(first, std::forward<Args>(args)...) {}