#pragma once

#include "../object.h"

template <typename T>
class Observable : public virtual ref<T>
{

public:
    Observable() : ref<T>(nullptr) {}
    Observable(std::nullptr_t) : ref<T>(nullptr) {}
    template <typename R>
    Observable(ref<R> ref) : ref<T>(ref) {}
    template <typename R>
    Observable(Observable<R> other) : ref<T>(other) {}

    Function<void()> willChange;
    Function<void()> didChanged;

    T *operator->() const { return ref<T>::operator->(); }

    template <typename R>
    Observable<T> &operator=(Observable<R> other)
    {
        if (willChange != nullptr)
            willChange();
        ref<T>::operator=(other);
        if (didChanged != nullptr)
            didChanged();
        return *this;
    }

    template <typename R>
    Observable<T> &operator=(ref<R> ref)
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
