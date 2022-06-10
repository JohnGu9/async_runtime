#pragma once
#include "base.h"
#include <unordered_map>

template <typename Key, typename Value>
class HashMap : public Map<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    std::unordered_map<Key, Value> _container;
    using super = std::map<Key, Value>;
    using T = typename super::value_type;

public:
    class HashMapConstIterator : public ConstIterator<T>
    {
        using iterator = typename std::unordered_map<Key, Value>::const_iterator;

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

    class HashMapIterator : public Iterator<T>
    {
        using iterator = typename std::unordered_map<Key, Value>::iterator;

    public:
        const iterator _it;
        HashMapIterator(const iterator it) : _it(it) {}

        ref<Iterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<HashMap<Key, Value>::HashMapIterator>(std::move(++copy));
        }

        T &value() const override { return const_cast<T &>(*(this->_it)); }

        bool operator==(ref<Object> other) override;

        ref<ConstIterator<T>> toConst() const override
        {
            return Object::create<HashMap<Key, Value>::HashMapConstIterator>(_it);
        }
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

    bool contain(const T &other) const
    {
        auto it = _container.find(other.first);
        if (it == _container.end())
            return false;
        return it->second == other.second;
    }

    bool contain(const Key &other) const
    {
        auto it = _container.find(other);
        return it != _container.end();
    }

    ref<Iterator<T>> find(const Key &key) override
    {
        return Object::create<HashMap<Key, Value>::HashMapIterator>(_container.find(key));
    }

    ref<ConstIterator<T>> find(const Key &key) const override
    {
        return Object::create<HashMap<Key, Value>::HashMapConstIterator>(_container.find(key));
    }

    Value &operator[](const Key &key)
    {
        return _container[key];
    }

    size_t size() const override { return _container.size(); }

    ref<Iterator<T>> begin() override
    {
        return Object::create<HashMap<Key, Value>::HashMapIterator>(_container.begin());
    }

    ref<Iterator<T>> end() override
    {
        return Object::create<HashMap<Key, Value>::HashMapIterator>(_container.end());
    }

    ref<Iterator<T>> erase(ref<Iterator<T>> it) override
    {
        auto iterator = it->template covariant<HashMap<Key, Value>::HashMapIterator>();
        return Object::create<HashMap<Key, Value>::HashMapIterator>(_container.erase(iterator->_it));
    }

    bool add(const T &value) override
    {
        _container.emplace(value.first, value.second);
        return true;
    }

    bool add(T &&value) override
    {
        _container.emplace(std::move(value.first), std::move(value.second));
        return true;
    }

    bool remove(const T &value) override
    {
        auto it = _container.find(value.first);
        if (it == _container.end())
            return false;
        _container.erase(it);
        return true;
    }

    bool removeAll(const T &value) override
    {
        return this->remove(value);
    }

    bool remove(const Key &key) override
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
    if (auto ptr = dynamic_cast<HashMap<Key, Value>::HashMapIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<HashMap<Key, Value>::HashMapConstIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::HashMapIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<HashMap<Key, Value>::HashMapConstIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<HashMap<Key, Value>::HashMapIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}
