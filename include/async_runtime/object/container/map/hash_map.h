#pragma once
#include "base.h"

template <typename Key, typename Value>
class HashMap : public Map<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = Map<Key, Value>;
    using element_type = typename super::element_type;
    using container_type = std::unordered_set<element_type, _async_runtime::KeyHasher<element_type>, _async_runtime::KeyEqual<element_type>>;
    container_type _container;

    container_type &container() const
    {
        return const_cast<container_type &>(_container);
    }

public:
    class _ConstIterator : public ConstIterator<element_type>
    {
        using iterator = typename container_type::iterator;

    public:
        const iterator _it;
        _ConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<element_type>> next() const override
        {
            auto copy = _it;
            return Object::create<_ConstIterator>(std::move(++copy));
        }

        const element_type &value() const override { return *(this->_it); }

        bool operator==(option<Object> other) override;
    };

    HashMap() {}
    HashMap(const std::initializer_list<element_type> &list) : _container(list) {}
    HashMap(std::initializer_list<element_type> &&list) : _container(std::move(list)) {}

    ref<Map<Key, Value>> copy() const override
    {
        auto other = Object::create<HashMap<Key, Value>>();
        for (const auto &pair : _container)
            other->emplace(pair->first, pair->second);
        return other;
    }

    ref<ConstIterator<element_type>> find(const element_type &value) const override
    {
        auto it = const_cast<container_type &>(_container).find(value);
        if (*it == value)
            return Object::create<_ConstIterator>(it);
        return this->end();
    }

    ref<ConstIterator<element_type>> findKey(const Key &key) const override
    {
        return Object::create<_ConstIterator>(container().find(super::keyOnlyPairBuilder(key)));
    }

    const Value &operator[](const Key &key) const override
    {
        auto pair = super::keyOnlyPairBuilder(key);
        auto it = _container.find(pair);
        return (*it)->second;
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

    ref<ConstIterator<element_type>> begin() const override
    {
        return Object::create<_ConstIterator>(container().begin());
    }

    ref<ConstIterator<element_type>> end() const override
    {
        return Object::create<_ConstIterator>(container().end());
    }

    ref<ConstIterator<element_type>> erase(ref<ConstIterator<element_type>> it) override
    {
        auto iterator = it.get()->template covariant<_ConstIterator>();
        return Object::create<_ConstIterator>(_container.erase(iterator.get()->_it));
    }

    bool emplace(const Key &key, const Value &value) override
    {
        auto result = _container.emplace(key, value);
        if (result.second == false)
        {
            const element_type &pair = *(result.first);
            pair->second = value;
        }
        return true;
    }

    bool emplace(Key &&key, Value &&value) override
    {
        auto result = _container.emplace(std::move(key), std::move(value));
        if (result.second == false)
        {
            const element_type &pair = *(result.first);
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
bool HashMap<Key, Value>::_ConstIterator::operator==(option<Object> other)
{
    if (auto ptr = dynamic_cast<_ConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    return false;
}
