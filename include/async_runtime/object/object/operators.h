#pragma once
#include "option.h"
#include "ref.h"

/**
 * @brief ref
 *
 */
template <typename T>
template <typename R>
bool ref<T>::operator==(const ref<R> &object1) const { return static_cast<const std::shared_ptr<T> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T>
template <typename R>
bool ref<T>::operator==(ref<R> &&object1) const { return static_cast<const std::shared_ptr<T> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T>
template <typename R>
bool ref<T>::operator==(const option<R> &object1) const { return static_cast<const std::shared_ptr<T> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T>
template <typename R>
bool ref<T>::operator==(option<R> &&object1) const { return static_cast<const std::shared_ptr<T> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }

/**
 * @brief option
 *
 */

template <typename T>
template <typename R>
bool option<T>::operator==(const option<R> &object1) const
{
    auto self = get();
    auto other = object1.get();
    if (self != nullptr && other != nullptr)
    {
        return self->shared_from_this() == other->shared_from_this();
    }
    return self == nullptr && other == nullptr;
}
template <typename T>
template <typename R>
bool option<T>::operator==(option<R> &&object1) const
{
    auto self = get();
    auto other = object1.get();
    if (self != nullptr && other != nullptr)
    {
        return self->shared_from_this() == other->shared_from_this();
    }
    return self == nullptr && other == nullptr;
}
template <typename T>
template <typename R>
bool option<T>::operator==(const ref<R> &object1) const
{
    auto self = get();
    if (self != nullptr)
    {
        return self->shared_from_this() == object1;
    }
    return false;
}
template <typename T>
template <typename R>
bool option<T>::operator==(ref<R> &&object1) const
{
    auto self = get();
    if (self != nullptr)
    {
        return self->shared_from_this() == object1;
    }
    return false;
}

/**
 * @brief ref<Fn<ReturnType(Args...)>>
 *
 */

template <typename ReturnType, class... Args>
template <typename R>
bool ref<Fn<ReturnType(Args...)>>::operator==(const ref<R> &object1) const { return static_cast<const std::shared_ptr<Fn<ReturnType(Args...)>> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename ReturnType, class... Args>
template <typename R>
bool ref<Fn<ReturnType(Args...)>>::operator==(ref<R> &&object1) const { return static_cast<const std::shared_ptr<Fn<ReturnType(Args...)>> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename ReturnType, class... Args>
template <typename R>
bool ref<Fn<ReturnType(Args...)>>::operator==(const option<R> &object1) const { return static_cast<const std::shared_ptr<Fn<ReturnType(Args...)>> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename ReturnType, class... Args>
template <typename R>
bool ref<Fn<ReturnType(Args...)>>::operator==(option<R> &&object1) const { return static_cast<const std::shared_ptr<Fn<ReturnType(Args...)>> &>(*this) == static_cast<const std::shared_ptr<R> &>(object1); }

/**
 * @brief ref<String>
 *
 */

template <typename R>
bool ref<String>::operator==(const ref<R> &object1) const
{
    auto other = object1.get();
    if (String *string = dynamic_cast<String *>(other))
    {
        return this->operator==(Object::cast<>(string));
    }
    return false;
}
template <typename R>
bool ref<String>::operator==(ref<R> &&object1) const
{
    auto other = object1.get();
    if (String *string = dynamic_cast<String *>(other))
    {
        return this->operator==(Object::cast<>(string));
    }
    return false;
}

template <typename R>
bool ref<String>::operator==(const option<R> &object1) const
{
    auto other = object1.get();
    if (String *string = dynamic_cast<String *>(other))
    {
        return this->operator==(Object::cast<>(string));
    }
    return false;
}
template <typename R>
bool ref<String>::operator==(option<R> &&object1) const
{
    auto other = object1.get();
    if (String *string = dynamic_cast<String *>(other))
    {
        return this->operator==(Object::cast<>(string));
    }
    return false;
}

/**
 * @brief Object
 *
 */

template <typename T>
std::ostream &operator<<(std::ostream &os, const ref<T> &object)
{
    object->toStringStream(os);
    return os;
}
