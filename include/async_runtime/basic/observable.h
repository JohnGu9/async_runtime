#pragma once

#include "../object.h"

template <typename T>
class observable : public virtual option<T>
{
    static void _invalid() {}

public:
    observable() : option<T>(nullptr) {}
    observable(std::nullptr_t) : option<T>(nullptr) {}

    template <typename R>
    observable(option<R> ref) : option<T>(ref) {}
    template <typename R>
    observable(observable<R> other) : option<T>(other) {}

    Function<void()> willChange = _invalid;
    Function<void()> didChanged = _invalid;

    T *operator->() const { return option<T>::operator->(); }

    template <typename R>
    observable<T> &operator=(observable<R> other)
    {
        willChange();
        option<T>::operator=(other);
        didChanged();
        return *this;
    }

    template <typename R>
    observable<T> &operator=(option<R> ref)
    {
        willChange();
        ref<T>::operator=(ref);
        didChanged();
        return *this;
    }

    template <typename R>
    bool operator==(observable<R> other)
    {
        return Object::identical(*this, other);
    }
};
