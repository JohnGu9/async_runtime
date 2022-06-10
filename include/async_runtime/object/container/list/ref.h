#pragma once

#include "../list.h"

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

    ref(const std::initializer_list<T> &list) : super(std::make_shared<DequeList<T>>(list)) {}
    ref(std::initializer_list<T> &&list) : super(std::make_shared<DequeList<T>>(std::move(list))) {}

    T &operator[](size_t index) const { return (*this)->operator[](index); }

    ref<ConstIterator<T>> begin() const { return (*this)->begin(); }
    ref<ConstIterator<T>> end() const { return (*this)->end(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
};

template <typename T>
ref<List<T>> List<T>::create()
{
    return Object::create<DequeList<T>>();
}

template <typename T>
template <typename R>
ref<List<R>> List<T>::map(Function<R(const T &)> fn) const
{
    auto mapped = Object::create<DequeList<R>>();
    this->forEach([&](const T &element)
                  { mapped->emplace(fn(element)); });
    return mapped;
}

template <typename T>
class lateref<List<T>> : public ref<List<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<List<T>>;

public:
    explicit lateref() : ref<List<T>>() {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<List<T>>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : ref<List<T>>(std::move(other)) {}

    lateref(const std::initializer_list<T> &list) : ref<List<T>>(list) {}
    lateref(std::initializer_list<T> &&list) : ref<List<T>>(std::move(list)) {}
};
