#pragma once

#include "../list.h"

#ifndef ASYNC_RUNTIME_CUSTOM_LIST_CONSTRUCTOR
namespace _async_runtime
{
    template <typename T>
    using DefaultList = DequeList<T>;
};
#else
ASYNC_RUNTIME_CUSTOM_LIST_CONSTRUCTOR
#endif

template <typename T>
class ref<List<T>> : public _async_runtime::RefImplement<List<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<List<T>>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    ref(const std::initializer_list<T> &list) : super(Object::create<_async_runtime::DefaultList<T>>(list)) {}
    ref(std::initializer_list<T> &&list) : super(Object::create<_async_runtime::DefaultList<T>>(std::move(list))) {}

    T &operator[](size_t index) const { return this->get()->operator[](index); }

    ref<Iterator<T>> begin() const { return this->get()->begin(); }
    ref<Iterator<T>> end() const { return this->get()->end(); }

protected:
    ref() noexcept : super() {}
    using super::super;
};

template <typename T>
ref<List<T>> List<T>::create() { return Object::create<_async_runtime::DefaultList<T>>(); }

template <typename T>
template <typename R>
ref<List<R>> List<T>::map(Function<R(const T &)> fn) const
{
    auto mapped = List<R>::create();
    this->forEach([&](const T &element)
                  { mapped->emplace(fn(element)); });
    return mapped;
}
