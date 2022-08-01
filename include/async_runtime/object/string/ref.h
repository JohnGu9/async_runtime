#pragma once
#include "string.h"
#include "string_native.h"

template <>
class ref<String> : public _async_runtime::RefImplement<String>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<String>;

public:
    using super::super;

    template <typename... Args, typename = decltype(String::Native(std::declval<Args>()...))>
    ref(Args &&...args) : super(Object::create<String::Native>(std::forward<Args>(args)...)) {}

protected:
    ref() noexcept = default;
};
