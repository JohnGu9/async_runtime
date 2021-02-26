#pragma once

#include <memory>

template <typename T>
using ref = std::shared_ptr<T>;

template <typename T>
using weakref = std::weak_ptr<T>;