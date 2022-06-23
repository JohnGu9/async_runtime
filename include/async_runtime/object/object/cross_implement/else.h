#pragma once

#include "../declare.h"

inline void _async_runtime::Else::ifElse(Function<void()> fn) const
{
    if (!this->_state)
        fn();
}
