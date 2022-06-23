#pragma once
#include "../option_implement.h"

template <typename T>
_async_runtime::Else _async_runtime::OptionImplement<T>::ifNotNull(Function<void(ref<T>)> fn) const noexcept
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
    {
        fn(ref<T>(ptr));
        return Else(true);
    }
    else
        return Else(false);
}

template <typename T>
ref<T> _async_runtime::OptionImplement<T>::ifNotNullElse(Function<ref<T>()> fn) const noexcept
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        return fn();
}

template <typename T>
ref<T> _async_runtime::OptionImplement<T>::assertNotNull() const noexcept(false)
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    RUNTIME_ASSERT(ptr != nullptr, std::string(typeid(*this).name()) + " assert not null on a null ref. ");
    return ref<T>(ptr);
}
