#pragma once

#include <deque>
#include <initializer_list>
#include <memory>

#include "../container.h"

template <typename T>
class List : public std::deque<T>, public Iterable<T>, public Removable<size_t>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename std::deque<T>::iterator;
    using const_iterator = typename std::deque<T>::const_iterator;
    static ref<List<T>> create();

    List() {}
    List(const List<T> &other) : std::deque<T>(other) {}

    List(std::initializer_list<T> &&list) : std::deque<T>(std::move(list)) {}
    List(const std::initializer_list<T> &list) : std::deque<T>(list) {}
    template <typename R>
    List(const std::initializer_list<R> &list) : std::deque<T>(list.begin(), list.end()) {}

    ref<List<T>> copy() const;
    template <typename R>
    ref<List<R>> map(Function<R(const T &)>) const;

    bool any(Function<bool(const T &)> fn) const override
    {
        for (const auto &iter : *this)
            if (fn(iter))
                return true;
        return false;
    }

    bool every(Function<bool(const T &)> fn) const override
    {
        for (const auto &iter : *this)
            if (!fn(iter))
                return false;
        return true;
    }

    void forEach(Function<void(const T &)> fn) const override
    {
        for (const auto &iter : *this)
            fn(iter);
    }

    bool remove(size_t index) override
    {
        assert(index < this->size());
        auto iter = this->begin() + index;
        this->erase(iter);
        return true;
    }

    void toStringStream(std::ostream &os) override
    {
        os << "List { ";
        for (const auto &element : *this)
            os << element << ", ";
        os << "}";
    }
};

template <typename T>
class ref<List<T>> : public _async_runtime::RefImplement<List<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename List<T>::iterator;
    using const_iterator = typename List<T>::const_iterator;

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<List<T>>(other) {}

    ref(const std::initializer_list<T> &list)
        : _async_runtime::RefImplement<List<T>>(std::make_shared<List<T>>(list)) {}

    ref(std::initializer_list<T> &&list)
        : _async_runtime::RefImplement<List<T>>(std::make_shared<List<T>>(std::move(list))) {}

    template <typename R>
    ref(const std::initializer_list<R> &list)
        : _async_runtime::RefImplement<List<T>>(std::make_shared<List<T>>(list)) {}

    template <typename R>
    ref(std::initializer_list<R> &&list)
        : _async_runtime::RefImplement<List<T>>(std::make_shared<List<T>>(list)) {}

    T &operator[](size_t index) const { return (*this)->operator[](index); }

    iterator begin()
    {
        return (*this)->begin();
    }

    const_iterator begin() const
    {
        return (*this)->begin();
    }

    iterator end()
    {
        return (*this)->end();
    }

    const_iterator end() const
    {
        return (*this)->end();
    }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<List<T>>(other) {}
};

template <typename T>
ref<List<T>> List<T>::create()
{
    return Object::create<List<T>>();
}

template <typename T>
ref<List<T>> List<T>::copy() const
{
    return Object::create<List<T>>(*this);
}

template <typename T>
template <typename R>
ref<List<R>> List<T>::map(Function<R(const T &)> fn) const
{
    auto mapped = Object::create<List<R>>();
    for (const auto &element : *this)
        mapped->emplace_back(fn(element));
    return mapped;
}

template <typename T>
class lateref<List<T>> : public ref<List<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename List<T>::iterator;
    using const_iterator = typename List<T>::const_iterator;

    explicit lateref() : ref<List<T>>() {}

    template <typename R, typename std::enable_if<std::is_base_of<List<T>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<List<T>>(other) {}

    lateref(const std::initializer_list<T> &list)
        : ref<List<T>>(list) {}

    lateref(std::initializer_list<T> &&list)
        : ref<List<T>>(std::move(list)) {}

    template <typename R>
    lateref(const std::initializer_list<R> &list)
        : ref<List<T>>(list) {}

    template <typename R>
    lateref(std::initializer_list<R> &&list)
        : ref<List<T>>(std::move(list)) {}
};