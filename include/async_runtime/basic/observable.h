#pragma once

#include "../object.h"

template <typename T>
class observable : public option<T>
{
    static void _invalid() {}

public:
    observable() : option<T>(nullptr) {}

    template <typename R>
    observable(option<R> ref) : option<T>(ref) {}

    Function<void()> willChange = _invalid;
    Function<void()> didChanged = _invalid;

    T *operator->() const { return std::shared_ptr<T>::operator->(); }
    T &operator*() const { return std::shared_ptr<T>::operator*(); }

    template <typename R>
    observable<T> &operator=(option<R> ref)
    {
        willChange();
        std::shared_ptr<T>::operator=(ref);
        didChanged();
        return *this;
    }

    observable<T> &operator=(std::nullptr_t)
    {
        willChange();
        std::shared_ptr<T>::operator=(nullptr);
        didChanged();
        return *this;
    }

    template <typename R>
    bool operator==(option<R> other)
    {
        return Object::identical(*this, other);
    }
};
