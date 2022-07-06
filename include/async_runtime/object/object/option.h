#pragma once
#include "ref.h"

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
class option : protected ref<T>, public _async_runtime::ToRefMixin<T>
{
    friend class Object;
    template <typename R>
    friend class option;
    using super = ref<T>;

public:
    using ref<T>::ref;

    option() noexcept : super() {}
    option(std::nullptr_t) noexcept : super() {}

    option(const ref<T> &other) noexcept : super(other) {}
    option(ref<T> &&other) noexcept : super(std::move(other)) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const option<R> &other) noexcept : super()
    {
        static_cast<std::shared_ptr<T> &>(*this) = static_cast<const std::shared_ptr<R> &>(other);
    }
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(option<R> &&other) noexcept : super()
    {
        static_cast<std::shared_ptr<T> &>(*this) = std::move(static_cast<std::shared_ptr<R> &>(other));
    }

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) noexcept : super()
    {
        static_cast<std::shared_ptr<T> &>(*this) = other;
    }
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(std::shared_ptr<R> &&other) noexcept : super()
    {
        static_cast<std::shared_ptr<T> &>(*this) = std::move(other);
    }

    T *get() const { return this->std::shared_ptr<T>::get(); }
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

    using super::operator<=;
    using super::operator>=;

    template <typename OTHER>
    bool operator!=(const OTHER &other) const { return !(*this == other); }
};
