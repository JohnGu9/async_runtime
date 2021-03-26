#pragma once

#include "../object.h"

template <typename T>
class Observable : public virtual option<T>
{

public:
    Observable() : option<T>(nullptr) {}
    Observable(std::nullptr_t) : option<T>(nullptr) {}
    template <typename R>
    Observable(option<R> ref) : option<T>(ref) {}
    template <typename R>
    Observable(Observable<R> other) : option<T>(other) {}

    Function<void()> willChange;
    Function<void()> didChanged;

    T *operator->() const { return option<T>::operator->(); }

    template <typename R>
    Observable<T> &operator=(Observable<R> other)
    {
        if (willChange != nullptr)
            willChange();
        option<T>::operator=(other);
        if (didChanged != nullptr)
            didChanged();
        return *this;
    }

    template <typename R>
    Observable<T> &operator=(option<R> ref)
    {
        if (willChange != nullptr)
            willChange();
        ref<T>::operator=(ref);
        if (didChanged != nullptr)
            didChanged();
        return *this;
    }

    template <typename R>
    bool operator==(Observable<R> other)
    {
        return Object::identical(*this, other);
    }
};
