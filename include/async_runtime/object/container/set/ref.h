#pragma once
#include "../set.h"

#ifndef ASYNC_RUNTIME_CUSTOM_SET_CONSTRUCTOR
namespace _async_runtime
{
    template <typename T>
    using DefaultSet = HashSet<T>;
};
#else
ASYNC_RUNTIME_CUSTOM_SET_CONSTRUCTOR
#endif

template <typename T>
class ref<Set<T>> : public _async_runtime::RefImplement<Set<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Set<T>>;

public:
    using super::super;

    ref(const std::initializer_list<T> &list) : super(Object::create<_async_runtime::DefaultSet<T>>(list)) {}
    ref(std::initializer_list<T> &&list) : super(Object::create<_async_runtime::DefaultSet<T>>(std::move(list))) {}

    ref<ConstIterator<T>> begin() const { return this->get()->begin(); }
    ref<ConstIterator<T>> end() const { return this->get()->end(); }

protected:
    ref() noexcept = default;
};

template <typename T>
ref<Set<T>> Set<T>::create() { return Object::create<_async_runtime::DefaultSet<T>>(); }

template <typename T>
template <typename R>
ref<Set<R>> Set<T>::map(Function<R(const T &)> fn) const
{
    auto mapped = Set<R>::create();
    for (const auto &element : *this)
        mapped->emplace(fn(element));
    return mapped;
}
