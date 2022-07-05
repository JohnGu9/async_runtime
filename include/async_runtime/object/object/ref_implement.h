#pragma once
#include "declare.h"

template <typename T>
class _async_runtime::RefImplement : protected std::shared_ptr<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = std::shared_ptr<T>;

public:
    using super::operator*;
    using super::operator->;

    T *get() const
    {
        DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
        return super::get();
    }

    template <typename OTHER>
    bool operator==(const OTHER &other) const;
    template <typename OTHER>
    bool operator!=(const OTHER &other) const { return !(*this == other); }

    template <typename OTHER>
    bool operator<(const OTHER &) const;
    template <typename OTHER>
    bool operator>(const OTHER &) const;

    template <typename OTHER>
    bool operator<=(const OTHER &other) const { return this->operator<(other) || this->operator==(other); }
    template <typename OTHER>
    bool operator>=(const OTHER &other) const { return this->operator>(other) || this->operator==(other); }

protected:
    RefImplement() noexcept {}
    RefImplement(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const std::shared_ptr<R> &other) noexcept : super(other)
    {
        DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
    }

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(std::shared_ptr<R> &&other) noexcept : super(std::move(other))
    {
        DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
    }
};
