#pragma once
#include "../weak_ref.h"

template <typename T>
_async_runtime::Else weakref<T>::ifNotNull(Function<void(ref<T>)> fn) const noexcept
{
    const auto ptr = std::weak_ptr<T>::lock();
    if (ptr != nullptr)
    {
        fn(ref<T>(ptr));
        return _async_runtime::Else(true);
    }
    else
        return _async_runtime::Else(false);
}

template <typename T>
ref<T> weakref<T>::ifNotNullElse(Function<ref<T>()> fn) const noexcept
{
    const auto ptr = std::weak_ptr<T>::lock();
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        return fn();
}

template <typename T>
ref<T> weakref<T>::assertNotNull() const noexcept(false)
{
    const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
    RUNTIME_ASSERT(ptr != nullptr, std::string(typeid(*this).name()) + " assert not null on a null ref. ");
    return ref<T>(ptr);
}

template <typename T>
option<T> weakref<T>::toOption() const noexcept
{
    return std::weak_ptr<T>::lock();
}
