#pragma once

#include "lateref.h"
#include "option.h"
#include "ref.h"
#include "weak_ref.h"

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
option<T>::option(const ref<R> &other)
    : _async_runtime::OptionImplement<T>(other) {}

template <typename T>
template <typename First, typename... Args>
option<T>::option(const First &first, Args &&...args) : _async_runtime::OptionImplement<T>(ref<T>(first, std::forward<Args>(args)...)) {}

template <typename T>
bool _async_runtime::OptionImplement<T>::isNotNull(ref<T> &object) const noexcept
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
    {
        object = ptr;
        return true;
    }
    else
        return false;
}

template <typename T>
ref<T> _async_runtime::OptionImplement<T>::isNotNullElse(Function<ref<T>()> fn) const noexcept
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        return fn();
}

template <typename T>
ref<T> _async_runtime::OptionImplement<T>::assertNotNull() const
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
}

template <typename T>
bool weakref<T>::isNotNull(ref<T> &object) const noexcept
{
    const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
    if (ptr != nullptr)
    {
        object = ptr;
        return true;
    }
    else
        return false;
}

template <typename T>
ref<T> weakref<T>::isNotNullElse(Function<ref<T>()> fn) const noexcept
{
    const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        return fn();
}

template <typename T>
ref<T> weakref<T>::assertNotNull() const
{
    const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
}

template <typename T>
option<T> weakref<T>::toOption() const noexcept
{
    return option<T>(std::weak_ptr<T>::lock());
}

// Object implement

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
ref<T> Object::create(_Args &&...__args)
{
    finalref<T> object = std::make_shared<T>(std::forward<_Args>(__args)...);
    object->init();
    return object;
}

/**
 * @brief detach let ref force to release the refences of object
 * object may not dispose if there is any other ref of itself
 * be careful to use this function that may cause serious problem
 *
 * @tparam T
 * @tparam *
 * @param object the ref need to force to release
 */
template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
void Object::detach(ref<T> &object) noexcept
{
    static_cast<std::shared_ptr<T> &>(object) = nullptr;
}

/**
 * @brief Object::identical
 *
 * @alarm use operator==(const std::std::shared_ptr&, const std::std::shared_ptr&) to make pointer check correctly
 * if using own raw pointer compare, the result maybe vary depending on platforms
 * on much platforms, object raw pointer value depending on current casted type
 *
 */
template <typename T0, typename T1>
bool Object::identical(const option<T0> &lhs, const option<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

template <typename T0, typename T1>
bool Object::identical(const ref<T0> &lhs, const ref<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

template <typename T0, typename T1>
bool Object::identical(const ref<T0> &lhs, const option<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

template <typename T0, typename T1>
bool Object::identical(const option<T0> &lhs, const ref<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

/**
 * @brief Object::equal
 *
 */
template <typename T0, typename T1>
bool Object::equal(const option<T0> &lhs, const option<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    else if (!Object::isNull<>(lhs) && !Object::isNull<>(rhs))
        return lhs.get()->operator==(Object::cast<>(rhs.get()));
    return false;
}

template <typename T0, typename T1>
bool Object::equal(const ref<T0> &lhs, const ref<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    return lhs.get()->operator==(rhs);
}

template <typename T0, typename T1>
bool Object::equal(const ref<T0> &lhs, const option<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    else if (Object::isNull<>(rhs))
        return false;
    return lhs.get()->operator==(Object::cast<>(rhs.get()));
}

template <typename T0, typename T1>
bool Object::equal(const option<T0> &lhs, const ref<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    else if (Object::isNull<>(lhs))
        return false;
    return lhs.get()->operator==(rhs);
}

/**
 * @brief Object::isNull
 *
 */
template <typename T>
bool Object::isNull(const option<T> &object) noexcept
{
    return static_cast<const std::shared_ptr<T> &>(object) == nullptr;
}

template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
ref<T> Object::cast(R *other)
{
#ifndef NDEBUG
    assert(other != nullptr);
#endif
    return std::shared_ptr<T>(static_cast<std::enable_shared_from_this<Object> *>(other)->shared_from_this(), static_cast<T *>(other));
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
ref<T> Object::cast(T *other)
{
#ifndef NDEBUG
    assert(other != nullptr);
#endif
    return std::shared_ptr<T>(static_cast<std::enable_shared_from_this<Object> *>(other)->shared_from_this(), other);
}

template <typename T>
option<T> Object::cast() noexcept
{
    return std::dynamic_pointer_cast<T>(this->std::enable_shared_from_this<Object>::shared_from_this());
}

template <typename T>
ref<T> Object::covariant() noexcept(false)
{
    if (T *castedPointer = dynamic_cast<T *>(this))
        return Object::cast<>(castedPointer);
    else
    {
        std::stringstream ss;
        ss << "Invalid type covariant from [" << typeid(*this).name() << "] to [" << typeid(T).name() << "]";
        throw std::runtime_error(ss.str());
    }
}
