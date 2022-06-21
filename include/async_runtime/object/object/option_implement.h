#pragma once
#include "declare.h"

template <typename T>
class _async_runtime::OptionImplement : protected std::shared_ptr<T>, public _async_runtime::ToRefMixin<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = std::shared_ptr<T>;

public:
    Else ifNotNull(Function<void(ref<T>)> fn) const noexcept override;
    ref<T> ifNotNullElse(Function<ref<T>()> fn) const noexcept override;
    ref<T> assertNotNull() const noexcept(false) override;

    T *get() const { return super::get(); }
    T *operator->() const = delete;
    T &operator*() const = delete;
    operator bool() const = delete;

protected:
    OptionImplement() noexcept {}
    OptionImplement(std::nullptr_t) noexcept : super(nullptr) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    OptionImplement(const std::shared_ptr<R> &other) noexcept : super(other){};
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    OptionImplement(std::shared_ptr<R> &&other) noexcept : super(other){};
};
