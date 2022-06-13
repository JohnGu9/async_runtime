#pragma once

#include "../container.h"

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

template <typename T>
class ref<ConstIterator<T>> : public _async_runtime::RefImplement<ConstIterator<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<ConstIterator<T>>;
    using element_type = ConstIterator<T>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(other) {}

    ref<element_type> &operator++()
    {
        *this = (*this)->next();
        return *this;
    }

    ref<element_type> operator++(int)
    {
        auto other = *this;
        operator++();
        return other;
    }

    const T &operator*() const { return (*this)->value(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
};

template <typename T>
class ref<Iterator<T>> : public _async_runtime::RefImplement<Iterator<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Iterator<T>>;
    using element_type = Iterator<T>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(other) {}

    ref<element_type> &operator++()
    {
        *this = (*this)->next();
        return *this;
    }

    ref<element_type> operator++(int)
    {
        auto other = *this;
        operator++();
        return other;
    }

    T &operator*() const { return (*this)->value(); }

    operator ref<ConstIterator<T>>() const { return (*this)->toConst(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
};
