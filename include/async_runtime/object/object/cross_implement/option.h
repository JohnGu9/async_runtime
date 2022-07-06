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

//

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type *>
bool option<T>::operator==(const OTHER &other) const
{
    if (this->get() == nullptr)
        return false;
    return this->super::operator==(other);
}

template <typename T>
template <typename R>
bool option<T>::operator==(const option<R> &other) const
{
    if (this->get() != nullptr)
        return this->super::operator==(other);
    else if (other.get() != nullptr)
        return (&other)->ref<R>::operator==(*this);
    return true;
}

template <typename T>
bool option<T>::operator==(const std::nullptr_t &) const
{
    if (this->get() == nullptr)
        return true;
    return this->super::operator==(option<Object>());
}

//

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type *>
bool option<T>::operator<(const OTHER &other) const
{
    if (this->get() == nullptr)
        return true;
    return this->super::operator<(other);
}

template <typename T>
template <typename R>
bool option<T>::operator<(const option<R> &other) const
{
    if (this->get() != nullptr)
        return this->super::operator<(other);
    else if (other.get() != nullptr)
        return (&other)->ref<R>::operator>(*this);
    return false;
}

template <typename T>
bool option<T>::operator<(const std::nullptr_t &) const
{
    if (this->get() == nullptr)
        return false;
    return this->super::operator<(option<Object>());
}

//

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type *>
bool option<T>::operator>(const OTHER &other) const
{
    if (this->get() == nullptr)
        return true;
    return this->super::operator>(other);
}

template <typename T>
template <typename R>
bool option<T>::operator>(const option<R> &other) const
{
    if (this->get() != nullptr)
        return this->super::operator>(other);
    else if (other.get() != nullptr)
        return (&other)->ref<R>::operator<(*this);
    return false;
}

template <typename T>
bool option<T>::operator>(const std::nullptr_t &) const
{
    if (this->get() == nullptr)
        return false;
    return this->super::operator>(option<Object>());
}
