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
        virtual bool isNotNull(ref<T> &) const = 0;
        virtual ref<T> isNotNullElse(Function<ref<T>()>) const = 0;
        virtual ref<T> assertNotNull() const = 0;
    };
};
