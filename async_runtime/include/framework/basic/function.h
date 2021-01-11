#pragma once

#include <functional>
#include <type_traits>

#include "../object.h"

class Function : public virtual Object
{
};

template <typename T>
using Fn =  std::function<T>;
