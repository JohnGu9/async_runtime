#pragma once
#include "declare.h"

template <typename T>
class _async_runtime::OptionImplement : protected std::shared_ptr<T>, public _async_runtime::ToRefMixin<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    bool isNotNull(ref<T> &) const noexcept override;
    ref<T> isNotNullElse(Function<ref<T>()>) const noexcept override;
    ref<T> assertNotNull() const override;
    size_t hashCode() const
    {
        static const auto hs = std::hash<std::shared_ptr<T>>();
        return hs(static_cast<const std::shared_ptr<T> &>(*this));
    }

    T *get() const { return std::shared_ptr<T>::get(); }
    T *operator->() const = delete;
    T &operator*() const = delete;
    operator bool() const = delete;

protected:
    OptionImplement() {}
    OptionImplement(std::nullptr_t) : std::shared_ptr<T>(nullptr) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    OptionImplement(const std::shared_ptr<R> &other) : std::shared_ptr<T>(other){};
};
