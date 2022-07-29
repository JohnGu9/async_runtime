#pragma once
#include "lateref.h"

namespace _async_runtime
{
    template <typename T>
    class OptionFilter;

    template <>
    class OptionFilter<std::nullptr_t> : public std::false_type
    {
    };

    template <typename T>
    class OptionFilter<option<T>> : public std::false_type
    {
    };

    template <typename T>
    class OptionFilter : public std::true_type
    {
    };
};

template <typename T>
class option : protected lateref<T>, public _async_runtime::ToRefMixin<T>
{
    friend class Object;
    template <typename R>
    friend class option;
    using super = lateref<T>;

public:
    using super::super;
    explicit option() noexcept = default; // MSVC bug patch: can't inherit no argument constructor from parent
    option(std::nullptr_t) noexcept : super() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const option<R> &other) noexcept : super(static_cast<const std::shared_ptr<R> &>(other)) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(option<R> &&other) noexcept : super(std::move(static_cast<std::shared_ptr<R> &>(other))) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(std::shared_ptr<R> &&other) noexcept : super(std::move(other)) {}

    using super::get;
    _async_runtime::Else ifNotNull(Function<void(ref<T>)> fn) const noexcept final;
    ref<T> ifNotNullElse(Function<ref<T>()> fn) const noexcept final;
    ref<T> assertNotNull() const noexcept(false) final;

    template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type * = nullptr>
    bool operator==(const OTHER &) const;
    template <typename R>
    bool operator==(const option<R> &) const;
    bool operator==(const std::nullptr_t &) const;

    template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type * = nullptr>
    bool operator<(const OTHER &) const;
    template <typename R>
    bool operator<(const option<R> &) const;
    bool operator<(const std::nullptr_t &) const;

    template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type * = nullptr>
    bool operator>(const OTHER &) const;
    template <typename R>
    bool operator>(const option<R> &) const;
    bool operator>(const std::nullptr_t &) const;

    template <typename OTHER>
    bool operator!=(const OTHER &other) const { return !this->operator==(other); }
    template <typename OTHER>
    bool operator<=(const OTHER &other) const { return this->operator<(other) || this->operator==(other); }
    template <typename OTHER>
    bool operator>=(const OTHER &other) const { return this->operator>(other) || this->operator==(other); }
};
