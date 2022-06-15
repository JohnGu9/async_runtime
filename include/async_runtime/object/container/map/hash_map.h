#pragma once
#include "base.h"
#include <unordered_map>

template <typename Key, typename Value>
class HashMap : public Map<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    std::unordered_map<Key, Value> _container;

    using super = Map<Key, Value>;
    using T = typename super::T;

public:
    class HashMapConstIterator : public ConstIterator<T>
    {
        using iterator = typename std::unordered_map<Key, Value>::iterator;

    public:
        const iterator _it;
        HashMapConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<HashMap<Key, Value>::HashMapConstIterator>(std::move(++copy));
        }

        const T &value() const override { return *(this->_it); }

        bool operator==(ref<Object> other) override;
    };

    HashMap() {}
    HashMap(std::initializer_list<T> &&list) : _container(std::move(list)) {}
    HashMap(const std::initializer_list<T> &list) : _container(list) {}

    ref<Map<Key, Value>> copy() const override
    {
        auto other = Object::create<HashMap<Key, Value>>();
        other->_container = _container;
        return other;
    }

    ref<ConstIterator<T>> findKey(const Key &key) const override
    {
        return Object::create<HashMap<Key, Value>::HashMapConstIterator>(
            const_cast<std::unordered_map<Key, Value> &>(_container).find(key));
    }

    Value &operator[](const Key &key) override
    {
        return _container[key];
    }

    size_t size() const override { return _container.size(); }

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<HashMap<Key, Value>::HashMapConstIterator>(
            const_cast<std::unordered_map<Key, Value> &>(_container).begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<HashMap<Key, Value>::HashMapConstIterator>(
            const_cast<std::unordered_map<Key, Value> &>(_container).end());
    }

    ref<ConstIterator<T>> erase(ref<ConstIterator<T>> it) override
    {
        auto iterator = it->template covariant<HashMap<Key, Value>::HashMapConstIterator>();
        return Object::create<HashMap<Key, Value>::HashMapConstIterator>(_container.erase(iterator->_it));
    }

    bool emplace(const Key &key, const Value &value) override
    {
        auto result = _container.emplace(key, value);
        if (result.second == false)
            result.first->second = value;
        return true;
    }

    bool emplace(Key &&key, Value &&value) override
    {
        auto result = _container.emplace(std::move(key), std::move(value));
        if (result.second == false)
            result.first->second = value;
        return true;
    }

    bool removeKey(const Key &key) override
    {
        auto it = _container.find(key);
        if (it == _container.end())
            return false;
        _container.erase(it);
        return true;
    }

    void clear() override { _container.clear(); }
};

template <typename Key, typename Value>
bool HashMap<Key, Value>::HashMapConstIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<HashMap<Key, Value>::HashMapConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    return false;
}
