#pragma once

#include <initializer_list>
#include <memory>
#include <unordered_map>

template <typename Key, typename Value>
class Map : public std::shared_ptr<std::unordered_map<Key, Value>>, public Lock::WithLockMixin
{
    explicit Map(std::nullptr_t) : std::shared_ptr<std::unordered_map<Key, Value>>(std::make_shared<std::unordered_map<Key, Value>>()) {}

public:
    using iterator = typename std::unordered_map<Key, Value>::iterator;
    using const_iterator = typename std::unordered_map<Key, Value>::const_iterator;
    using value_type = typename std::unordered_map<Key, Value>::value_type;

    static Map<Key, Value> empty() { return Map<Key, Value>(nullptr); }
    explicit Map() {}
    Map(const Map<Key, Value> &other)
        : std::shared_ptr<std::unordered_map<Key, Value>>(std::make_shared<std::unordered_map<Key, Value>>(*other)), Lock::WithLockMixin(other) { assert(*this); }
    Map(std::initializer_list<value_type> &&ls)
        : std::shared_ptr<std::unordered_map<Key, Value>>(std::make_shared<std::unordered_map<Key, Value>>(std::forward<std::initializer_list<value_type>>(ls))) { assert(*this); }

    Map<Key, Value> &operator=(std::initializer_list<value_type> &&ls)
    {
        std::shared_ptr<std::unordered_map<Key, Value>>::operator=(
            std::make_shared<std::unordered_map<Key, Value>>(std::forward<std::initializer_list<value_type>>(ls)));
        assert(*this);
        return *this;
    }

    Map<Key, Value> &operator=(const Map<Key, Value> &other)
    {
        std::shared_ptr<std::unordered_map<Key, Value>>::operator=(
            static_cast<const std::shared_ptr<std::unordered_map<Key, Value>> &>(other));
        Lock::WithLockMixin::operator=(other);
        return *this;
    }

    Map<Key, Value> &operator=(std::nullptr_t t)
    {
        std::shared_ptr<std::unordered_map<Key, Value>>::operator=(t);
        return *this;
    }

    Value &operator[](const Key &key) { return (*this)->operator[](key); }
    Value &operator[](const Key &key) const { return (*this)->operator[](key); }
    Value &operator[](Key &&key) { return (*this)->operator[](std::forward<Key>(key)); }
    Value &operator[](Key &&key) const { return (*this)->operator[](std::forward<Key>(key)); }

    iterator begin()
    {
        assert(*this);
        return (*this)->begin();
    }
    const_iterator begin() const
    {
        assert(*this);
        return (*this)->begin();
    }

    iterator end()
    {
        assert(*this);
        return (*this)->end();
    }
    const_iterator end() const
    {
        assert(*this);
        return (*this)->end();
    }

    Map<Key, Value> copy() const
    {
        return Map<Key, Value>(*this);
    }
};