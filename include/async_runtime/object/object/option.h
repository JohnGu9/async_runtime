#pragma once
#include "ref.h"

template <typename T>
class option : protected ref<T>, public _async_runtime::ToRefMixin<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<T>;

public:
    using ref<T>::ref;

    option() noexcept : super() {}
    option(std::nullptr_t) noexcept : super() {}

    option(const ref<T> &other) noexcept : super(other) {}
    option(ref<T> &&other) noexcept : super(std::move(other)) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const option<R> &other) noexcept : super{other == nullptr ? super() : super(other)} {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(option<R> &&other) noexcept : super{other == nullptr ? super() : super(std::move(other))} {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) noexcept : super{other == nullptr ? super() : super(other)} {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(std::shared_ptr<R> &&other) noexcept : super{other == nullptr ? super() : super(std::move(other))} {}

    T *get() const { return this->std::shared_ptr<T>::get(); }
    _async_runtime::Else ifNotNull(Function<void(ref<T>)> fn) const noexcept final;
    ref<T> ifNotNullElse(Function<ref<T>()> fn) const noexcept final;
    ref<T> assertNotNull() const noexcept(false) final;
};
