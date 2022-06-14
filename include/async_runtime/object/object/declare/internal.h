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

    class Else
    {
        const bool _state;

    public:
        Else(const bool state) : _state(state) {}
        Else(const Else &other) : _state(other._state) {}
        Else(Else &&other) : _state(other._state) {}

        void ifElse(Function<void()> fn) const;
    };

    template <typename T>
    class ToRefMixin
    {
    public:
        virtual ~ToRefMixin() {}
        virtual Else ifNotNull(Function<void(ref<T>)> fn) const noexcept = 0;
        virtual ref<T> ifNotNullElse(Function<ref<T>()> fn) const noexcept = 0; // if self it not null, return self ref; otherwise return the value come from fn
        virtual ref<T> assertNotNull() const noexcept(false) = 0;               // if self is null, api will throw a std::runtime_error
    };

    extern size_t ostreamStackDepth;
};
