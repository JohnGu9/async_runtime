#pragma once
#include "iterable.h"
#include "pair.h"

template <typename T>
class ref<Iterable<T>> : public _async_runtime::RefImplement<Iterable<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Iterable<T>>;
    using element_type = Iterable<T>;

public:
    using super::super;
    ref(std::initializer_list<T> list);

protected:
    ref() noexcept = default;
};
