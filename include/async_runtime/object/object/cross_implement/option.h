#pragma once

#include "../option.h"

template <typename T>
_async_runtime::Else option<T>::ifNotNull(Function<void(ref<T>)> fn) const noexcept
{
    const std::shared_ptr<T> &ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
    {
        fn(ref<T>(ptr));
        return _async_runtime::Else(true);
    }
    else
        return _async_runtime::Else(false);
}

template <typename T>
ref<T> option<T>::ifNotNullElse(Function<ref<T>()> fn) const noexcept
{
    const std::shared_ptr<T> &ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        return fn();
}

template <typename T>
ref<T> option<T>::assertNotNull() const noexcept(false)
{
    const std::shared_ptr<T> &ptr = static_cast<const std::shared_ptr<T> &>(*this);
    RUNTIME_ASSERT(ptr != nullptr, std::string(typeid(*this).name()) + " assert not null on a null ref. ");
    return ref<T>(ptr);
}
