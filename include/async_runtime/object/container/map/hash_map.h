#pragma once
#include "base.h"

template <typename Key, typename Value>
class HashMap : public Map<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = Map<Key, Value>;
    using Pair = typename super::Pair;
    using container_type = std::unordered_set<Pair, _async_runtime::KeyHasher<Pair>, _async_runtime::KeyEqual<Pair>>;
    container_type _container;

public:
    class HashMapConstIterator : public ConstIterator<Pair>
    {
        using iterator = typename container_type::iterator;

    public:
        const iterator _it;
        HashMapConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<Pair>> next() const override
        {
            auto copy = _it;
            return Object::create<HashMap<Key, Value>::HashMapConstIterator>(std::move(++copy));
        }

        const Pair &value() const override { return *(this->_it); }

        bool operator==(ref<Object> other) override;
    };

    HashMap() {}
    HashMap(const std::initializer_list<Pair> &list) : _container(list) {}
    HashMap(std::initializer_list<Pair> &&list) : _container(std::move(list)) {}

    ref<Map<Key, Value>> copy() const override
    {
        auto other = Object::create<HashMap<Key, Value>>();
        for (const auto &pair : _container)
            other->emplace(pair->first, pair->second);
        return other;
    }

    ref<ConstIterator<Pair>> find(const Pair &value) const override
    {
        auto it = const_cast<container_type &>(_container).find(value);
        if (*it == value)
            return Object::create<HashMapConstIterator>(it);
        return this->end();
    }

    ref<ConstIterator<Pair>> findKey(const Key &key) const override
    {
        return Object::create<HashMapConstIterator>(
            const_cast<container_type &>(_container).find(super::keyOnlyPairBuilder(key)));
    }

    Value &operator[](const Key &key) override
    {
        auto pair = super::keyOnlyPairBuilder(key);
        auto it = _container.find(pair);
        if (it == _container.end())
        {
            _container.insert(pair);
            return pair->second;
        }
        return (*it)->second;
    }

    size_t size() const override { return _container.size(); }

    ref<ConstIterator<Pair>> begin() const override
    {
        return Object::create<HashMapConstIterator>(
            const_cast<container_type &>(_container).begin());
    }

    ref<ConstIterator<Pair>> end() const override
    {
        return Object::create<HashMapConstIterator>(
            const_cast<container_type &>(_container).end());
    }

    ref<ConstIterator<Pair>> erase(ref<ConstIterator<Pair>> it) override
    {
        auto iterator = it.get()->template covariant<HashMapConstIterator>();
        return Object::create<HashMapConstIterator>(_container.erase(iterator.get()->_it));
    }

    bool emplace(const Key &key, const Value &value) override
    {
        auto result = _container.emplace(key, value);
        if (result.second == false)
        {
            const Pair &pair = *(result.first);
            pair->second = value;
        }
        return true;
    }

    bool emplace(Key &&key, Value &&value) override
    {
        auto result = _container.emplace(std::move(key), std::move(value));
        if (result.second == false)
        {
            const Pair &pair = *(result.first);
            pair->second = std::move(value);
        }
        return true;
    }

    bool removeKey(const Key &key) override
    {
        auto it = _container.find(super::keyOnlyPairBuilder(key));
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
