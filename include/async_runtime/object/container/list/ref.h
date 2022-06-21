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
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(const std::initializer_list<T> &list) : super(Object::create<_async_runtime::DefaultList<T>>(list)) {}
    ref(std::initializer_list<T> &&list) : super(Object::create<_async_runtime::DefaultList<T>>(std::move(list))) {}

    T &operator[](size_t index) const { return (*this)->operator[](index); }

    ref<Iterator<T>> begin() const { return (*this)->begin(); }
    ref<Iterator<T>> end() const { return (*this)->end(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(std::shared_ptr<R> &&other) : super(std::move(other)) {}
};

template <typename T>
class lateref<List<T>> : public ref<List<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<List<T>>;

public:
    explicit lateref() : ref<List<T>>() {}
    lateref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<List<T>>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : ref<List<T>>(std::move(other)) {}

    lateref(const std::initializer_list<T> &list) : ref<List<T>>(list) {}
    lateref(std::initializer_list<T> &&list) : ref<List<T>>(std::move(list)) {}

    T &operator[](size_t index) const { return (*this)->operator[](index); }

    ref<Iterator<T>> begin() const { return (*this)->begin(); }
    ref<Iterator<T>> end() const { return (*this)->end(); }
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
