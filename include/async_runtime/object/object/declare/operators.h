#pragma once
#include "basic.h"
#include <iostream>
#include <sstream>

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &);

template <typename T>
std::ostream &operator<<(std::ostream &os, ref<T> &&);

template <typename T>
std::ostream &operator<<(std::ostream &os, const option<T> &);

template <typename T>
std::ostream &operator<<(std::ostream &os, option<T> &&);
