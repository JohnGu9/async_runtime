#pragma once

#include "../object.h"

template <typename T>
class Observable : public virtual Object::Ref<T>
{

public:
    Observable() : Object::Ref<T>(nullptr) {}
    Observable(std::nullptr_t) : Object::Ref<T>(nullptr) {}
    template <typename R>
    Observable(Object::Ref<R> ref) : Object::Ref<T>(ref) {}
    template <typename R>
    Observable(Observable<R> other) : Object::Ref<T>(other) {}

    Function<void()> willChange;
    Function<void()> didChanged;

    T *operator->() const { return Object::Ref<T>::operator->(); }

    template <typename R>
    Observable<T> &operator=(Observable<R> other)
    {
        if (willChange != nullptr)
            willChange();
        Object::Ref<T>::operator=(other);
        if (didChanged != nullptr)
            didChanged();
        return *this;
    }

    template <typename R>
    Observable<T> &operator=(Object::Ref<R> ref)
    {
        if (willChange != nullptr)
            willChange();
        Object::Ref<T>::operator=(ref);
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
