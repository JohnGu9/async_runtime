#pragma once
#include "declare.h"

namespace _async_runtime
{
    template <typename T>
    class RefFilter;

    template <>
    class RefFilter<std::nullptr_t> : public std::false_type
    {
    };

    template <typename T>
    class RefFilter : public std::true_type
    {
    };
};

template <typename T>
class _async_runtime::RefImplement : protected std::shared_ptr<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = std::shared_ptr<T>;

public:
    using super::operator*;
    using super::operator->;
    using super::get;

    template <typename OTHER, typename std::enable_if<_async_runtime::RefFilter<OTHER>::value>::type * = nullptr>
    bool operator==(const OTHER &) const;
    bool operator==(const std::nullptr_t &) const;

    template <typename OTHER, typename std::enable_if<_async_runtime::RefFilter<OTHER>::value>::type * = nullptr>
    bool operator<(const OTHER &) const;
    bool operator<(const std::nullptr_t &) const;
    template <typename OTHER, typename std::enable_if<_async_runtime::RefFilter<OTHER>::value>::type * = nullptr>
    bool operator>(const OTHER &) const;
    bool operator>(const std::nullptr_t &) const;

    template <typename OTHER>
    bool operator!=(const OTHER &other) const { return !this->operator==(other); }
    template <typename OTHER>
    bool operator<=(const OTHER &other) const { return this->operator<(other) || this->operator==(other); }
    template <typename OTHER>
    bool operator>=(const OTHER &other) const { return this->operator>(other) || this->operator==(other); }

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const ref<R> &other) noexcept;
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(ref<R> &&other) noexcept;

    RefImplement(std::nullptr_t) = delete;

protected:
    RefImplement() noexcept = default;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const std::shared_ptr<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(std::shared_ptr<R> &&other) noexcept : super(std::move(other)) {}
};
