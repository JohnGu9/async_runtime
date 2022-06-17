#pragma once
#include "base.h"

template <typename Key, typename Value>
class HashMap : public Map<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = Map<Key, Value>;
    using T = typename super::T;
    using std_pair = typename super::std_pair;
    using container_type = std::unordered_set<T, _async_runtime::KeyHasher<T>, _async_runtime::KeyEqual<T>>;
    container_type _container;

public:
    class HashMapConstIterator : public ConstIterator<T>
    {
        using iterator = typename container_type::iterator;

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
    HashMap(const std::initializer_list<std_pair> &list)
    {
        for (const std_pair &pair : list)
            _container.insert(T(pair));
    }
    HashMap(std::initializer_list<std_pair> &&list)
    {
        for (const std_pair &pair : list)
            _container.insert(T(std::move(pair)));
    }

    ref<Map<Key, Value>> copy() const override
    {
        auto other = Object::create<HashMap<Key, Value>>();
        other->_container = _container;
        return other;
    }

    ref<ConstIterator<T>> findKey(const Key &key) const override
    {
        return Object::create<typename HashMap<Key, Value>::HashMapConstIterator>(
            const_cast<container_type &>(_container).find(T(key)));
    }

    Value &operator[](const Key &key) override
    {
        auto pair = T(key);
        auto it = _container.find(pair);
        if (it == _container.end())
        {
            _container.insert(pair);
            return pair->second;
        }
        return (*it)->second;
    }

    size_t size() const override { return _container.size(); }

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<typename HashMap<Key, Value>::HashMapConstIterator>(
            const_cast<container_type &>(_container).begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<typename HashMap<Key, Value>::HashMapConstIterator>(
            const_cast<container_type &>(_container).end());
    }

    ref<ConstIterator<T>> erase(ref<ConstIterator<T>> it) override
    {
        auto iterator = it.get()->template covariant<typename HashMap<Key, Value>::HashMapConstIterator>();
        return Object::create<typename HashMap<Key, Value>::HashMapConstIterator>(_container.erase(iterator.get()->_it));
    }

    bool emplace(const Key &key, const Value &value) override
    {
        auto result = _container.emplace(key, value);
        if (result.second == false)
        {
            const T &pair = *(result.first);
            pair->second = value;
        }
        return true;
    }

    bool emplace(Key &&key, Value &&value) override
    {
        auto result = _container.emplace(std::move(key), std::move(value));
        if (result.second == false)
        {
            const T &pair = *(result.first);
            pair->second = std::move(value);
        }
        return true;
    }

    bool removeKey(const Key &key) override
    {
        auto it = _container.find(T(key));
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
    if (auto ptr = dynamic_cast<typename HashMap<Key, Value>::HashMapConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    return false;
}
