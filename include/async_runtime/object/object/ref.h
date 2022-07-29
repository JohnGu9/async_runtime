#pragma once
#include "ref_implement.h"

template <typename T>
class ref : public _async_runtime::RefImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<T>;

public:
    using super::super;

protected:
    ref() noexcept = default; // MSVC bug patch: can't inherit no argument constructor from parent
};
