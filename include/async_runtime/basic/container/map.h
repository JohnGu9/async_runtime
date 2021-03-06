#pragma once

#include <initializer_list>
#include <memory>
#include <unordered_map>

#include "../container.h"

template <typename Key, typename Value>
class Map : public std::unordered_map<Key, Value>, public Iterable<typename std::unordered_map<Key, Value>::value_type>, public Removable<Key>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename std::unordered_map<Key, Value>::iterator;
    using const_iterator = typename std::unordered_map<Key, Value>::const_iterator;
    using value_type = typename std::unordered_map<Key, Value>::value_type;

    Map() {}
    Map(const Map<Key, Value> &other) : std::unordered_map<Key, Value>(other) {}

    Map(std::initializer_list<value_type> &&list) : std::unordered_map<Key, Value>(std::move(list)) {}
    Map(const std::initializer_list<value_type> &list) : std::unordered_map<Key, Value>(list) {}
    template <typename R>
    Map(const std::initializer_list<R> &list) : std::unordered_map<Key, Value>(list.begin(), list.end()) {}

    ref<Map<Key, Value>> copy() const;

    bool any(Function<bool(const value_type &)> fn) const override
    {
        for (const auto &iter : *this)
            if (fn(iter))
                return true;
        return false;
    }

    bool every(Function<bool(const value_type &)> fn) const override
    {
        for (const auto &iter : *this)
            if (!fn(iter))
                return false;
        return true;
    }

    void forEach(Function<void(const value_type &)> fn) const override
    {
        for (const auto &iter : *this)
            fn(iter);
    }

    bool remove(Key v) override
    {
        auto iter = this->find(v);
        if (iter == this->end())
            return false;
        this->erase(iter);
        return true;
    }
};

template <typename Key, typename Value>
class ref<Map<Key, Value>> : public _async_runtime::RefImplement<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename Map<Key, Value>::iterator;
    using const_iterator = typename Map<Key, Value>::const_iterator;
    using value_type = typename Map<Key, Value>::value_type;

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<Map<Key, Value>>(other) {}

    ref(const std::initializer_list<value_type> &list)
        : _async_runtime::RefImplement<Map<Key, Value>>(std::make_shared<Map<Key, Value>>(list)) {}

    ref(std::initializer_list<value_type> &&list)
        : _async_runtime::RefImplement<Map<Key, Value>>(std::make_shared<Map<Key, Value>>(std::move(list))) {}

    template <typename R>
    ref(const std::initializer_list<R> &list)
        : _async_runtime::RefImplement<Map<Key, Value>>(std::make_shared<Map<Key, Value>>(list)) {}

    template <typename R>
    ref(std::initializer_list<R> &&list)
        : _async_runtime::RefImplement<Map<Key, Value>>(std::make_shared<Map<Key, Value>>(list)) {}

    template <typename... Args>
    Value &operator[](Args &&...key) const { return (*this)->operator[](std::forward<Args>(key)...); }

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

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<Map<Key, Value>>(other) {}
};

template <typename Key, typename Value>
ref<Map<Key, Value>> Map<Key, Value>::copy() const
{
    return Object::create<Map<Key, Value>>(*this);
}

template <typename Key, typename Value>
class lateref<Map<Key, Value>> : public ref<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename Map<Key, Value>::iterator;
    using const_iterator = typename Map<Key, Value>::const_iterator;
    using value_type = typename Map<Key, Value>::value_type;

    explicit lateref() : ref<Map<Key, Value>>() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<Map<Key, Value>>(other) {}

    lateref(const std::initializer_list<value_type> &list)
        : ref<Map<Key, Value>>(list) {}

    lateref(std::initializer_list<value_type> &&list)
        : ref<Map<Key, Value>>(std::move(list)) {}

    template <typename R>
    lateref(const std::initializer_list<R> &list)
        : ref<Map<Key, Value>>(list) {}

    template <typename R>
    lateref(std::initializer_list<R> &&list)
        : ref<Map<Key, Value>>(std::move(list)) {}
};