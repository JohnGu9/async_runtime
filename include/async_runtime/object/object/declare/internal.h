#pragma once

#include "basic.h"
#include "function.h"

// the namespace not for public usage
// _async_runtime just for async runtime internal usage
namespace _async_runtime
{
    template <typename T>
    class OptionImplement;

    template <typename T>
    class RefImplement;

    template <typename T>
    class ToRefMixin
    {
    public:
        virtual ~ToRefMixin() {}
        virtual bool isNotNull(ref<T> &other) const noexcept = 0;               // if self is not null, change the [other] to ref self, and return [true]; otherwise just return false
        virtual ref<T> isNotNullElse(Function<ref<T>()> fn) const noexcept = 0; // if self it not null, return self ref; otherwise return the value come from fn
        virtual ref<T> assertNotNull() const = 0;                               // if self is null, api will throw a std::runtime_error
    };
};
