#pragma once
#include "declare.h"

template <typename T>
class _async_runtime::RefImplement : protected std::shared_ptr<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    size_t hashCode() const
    {
        static const auto hs = std::hash<std::shared_ptr<T>>();
        return hs(static_cast<const std::shared_ptr<T> &>(*this));
    }

    T &operator*() const
    {
        DEBUG_ASSERT(std::shared_ptr<T>::get() && "ref Uninitiated NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::operator*();
    }

    T *operator->() const
    {
        DEBUG_ASSERT(std::shared_ptr<T>::get() && "ref Uninitiated NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::operator->();
    }

    T *get() const
    {
        DEBUG_ASSERT(std::shared_ptr<T>::get() && "ref Uninitiated NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::get();
    }

    std::shared_ptr<T> &operator=(std::nullptr_t) = delete;
    bool operator==(std::nullptr_t) const = delete;
    bool operator!=(std::nullptr_t) const = delete;
    operator bool() const = delete;

protected:
    RefImplement() {}
    RefImplement(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const std::shared_ptr<R> &other) : std::shared_ptr<T>(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(std::shared_ptr<R> &&other) : std::shared_ptr<T>(other) {}
};
