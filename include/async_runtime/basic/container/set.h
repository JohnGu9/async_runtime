#pragma once

#include <initializer_list>
#include <memory>
#include <unordered_set>
#include "../container.h"

template <typename T>
class Set : public std::unordered_set<T>, public Iterable<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename std::unordered_set<T>::iterator;
    using const_iterator = typename std::unordered_set<T>::const_iterator;

    Set() {}
    Set(const Set<T> &other) : std::unordered_set<T>(other) {}

    Set(std::initializer_list<T> &&list) : std::unordered_set<T>(std::move(list)) {}
    Set(const std::initializer_list<T> &list) : std::unordered_set<T>(list) {}
    template <typename R>
    Set(const std::initializer_list<R> &list) : std::unordered_set<T>(list.begin(), list.end()) {}

    ref<Set<T>> copy() const;

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
};

template <typename T>
class ref<Set<T>> : public _async_runtime::RefImplement<Set<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename Set<T>::iterator;
    using const_iterator = typename Set<T>::const_iterator;

    template <typename R, typename std::enable_if<std::is_base_of<Set<T>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<Set<T>>(other) {}

    ref(const std::initializer_list<T> &list)
        : _async_runtime::RefImplement<Set<T>>(std::make_shared<Set<T>>(list)) {}

    ref(std::initializer_list<T> &&list)
        : _async_runtime::RefImplement<Set<T>>(std::make_shared<Set<T>>(std::move(list))) {}

    template <typename R>
    ref(const std::initializer_list<R> &list)
        : _async_runtime::RefImplement<Set<T>>(std::make_shared<Set<T>>(list)) {}

    template <typename R>
    ref(std::initializer_list<R> &&list)
        : _async_runtime::RefImplement<Set<T>>(std::make_shared<Set<T>>(list)) {}

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

    template <typename R, typename std::enable_if<std::is_base_of<Set<T>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<Set<T>>(other) {}
};

template <typename T>
ref<Set<T>> Set<T>::copy() const
{
    return Object::create<Set<T>>(*this);
}

template <typename T>
class lateref<Set<T>> : public ref<Set<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename Set<T>::iterator;
    using const_iterator = typename Set<T>::const_iterator;

    explicit lateref() : ref<Set<T>>() {}

    template <typename R, typename std::enable_if<std::is_base_of<Set<T>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<Set<T>>(other) {}

    lateref(const std::initializer_list<T> &list)
        : ref<Set<T>>(list) {}

    lateref(std::initializer_list<T> &&list)
        : ref<Set<T>>(std::move(list)) {}

    template <typename R>
    lateref(const std::initializer_list<R> &list)
        : ref<Set<T>>(list) {}

    template <typename R>
    lateref(std::initializer_list<R> &&list)
        : ref<Set<T>>(std::move(list)) {}

protected:
    template <typename R, typename std::enable_if<std::is_base_of<Set<T>, R>::value>::type * = nullptr>
    lateref(const std::shared_ptr<R> &other) : ref<Set<T>>(other) {}
};