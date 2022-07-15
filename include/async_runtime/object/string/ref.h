#pragma once

#include "string.h"
#include "string_native.h"

template <>
class ref<String> : public _async_runtime::RefImplement<String>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<String>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    template <typename... Args, typename = decltype(String::Native(std::declval<Args>()...))>
    ref(Args &&...args) : super(Object::create<String::Native>(std::forward<Args>(args)...)) {}

    const char &operator[](size_t index) const { return this->get()->operator[](index); }
    ref<ConstIterator<char>> begin() const { return this->get()->begin(); }
    ref<ConstIterator<char>> end() const { return this->get()->end(); }

    template <typename T>
    ref<String> operator+(const T &value) const { return String::connect(*this, value); }
    template <typename T>
    ref<String> operator+(T &&value) const { return String::connect(*this, std::move(value)); }

protected:
    ref() noexcept : super() {}
    using super::super;
};
