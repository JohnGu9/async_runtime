#pragma once
#include "declare.h"

template <typename T>
class _async_runtime::RefImplement : protected std::shared_ptr<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = std::shared_ptr<T>;

public:
    T &operator*() const
    {
        DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
        return super::operator*();
    }

    T *operator->() const
    {
        DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
        return super::operator->();
    }

    T *get() const
    {
        DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
        return super::get();
    }

    std::shared_ptr<T> &operator=(std::nullptr_t) = delete;
    bool operator==(std::nullptr_t) const = delete;
    bool operator!=(std::nullptr_t) const = delete;
    operator bool() const = delete;

protected:
    RefImplement() {}
    RefImplement(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const std::shared_ptr<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(std::shared_ptr<R> &&other) : super(other) {}
};
