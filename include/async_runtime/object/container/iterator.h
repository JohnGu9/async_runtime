#pragma once

#include "../object.h"
#include "declare.h"

template <typename T>
class ConstIterator : public virtual Object
{
public:
    virtual ref<ConstIterator<T>> next() const = 0;
    virtual const T &value() const = 0;
};

template <typename T>
class Iterator : public virtual Object
{
public:
    virtual ref<Iterator<T>> next() const = 0;
    virtual T &value() const = 0;
    virtual ref<ConstIterator<T>> toConst() const = 0;
};

#include <cstddef>
#include <iterator>

template <typename T>
class ref<ConstIterator<T>> : protected _async_runtime::RefImplement<ConstIterator<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<ConstIterator<T>>;
    using element_type = ConstIterator<T>;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = const T;
    using pointer = value_type *;
    using reference = value_type &;

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(other) {}

    element_type *get() const { return super::get(); }
    pointer operator->() const { return &((this->get())->value()); }
    reference operator*() const { return (this->get())->value(); }

    ref<element_type> &operator++()
    {
        *this = (this->get())->next();
        return *this;
    }

    ref<element_type> operator++(int)
    {
        auto other = *this;
        operator++();
        return other;
    }

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<ConstIterator<T>>::RefImplement;
};

template <typename T>
class ref<Iterator<T>> : protected _async_runtime::RefImplement<Iterator<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Iterator<T>>;
    using element_type = Iterator<T>;

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type *;
    using reference = value_type &;

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    element_type *get() const { return super::get(); }
    pointer operator->() const { return &((this->get())->value()); }
    reference operator*() const { return (this->get())->value(); }

    ref<element_type> &operator++()
    {
        *this = (this->get())->next();
        return *this;
    }

    ref<element_type> operator++(int)
    {
        auto other = *this;
        operator++();
        return other;
    }

    operator ref<ConstIterator<T>>() const { return (*this)->toConst(); }

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Iterator<T>>::RefImplement;
};
