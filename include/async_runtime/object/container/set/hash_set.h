#pragma once
#include "base.h"
#include <unordered_set>

template <typename T>
class HashSet : public Set<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    std::unordered_set<T> _container;

public:
    class HashSetConstIterator : public ConstIterator<T>
    {
        using iterator = typename std::unordered_set<T>::iterator;

    public:
        const iterator _it;
        HashSetConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<HashSet<T>::HashSetConstIterator>(std::move(++copy));
        }

        const T &value() const override { return *(this->_it); }

        bool operator==(ref<Object> other) override;
    };

    static ref<Set<T>> create();

    HashSet() {}
    HashSet(const Set<T> &other)
    {
        for (const T &value : other)
            _container.insert(value);
    }

    HashSet(std::initializer_list<T> &&list) : _container(std::move(list)) {}
    HashSet(const std::initializer_list<T> &list) : _container(list) {}

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<HashSet<T>::HashSetConstIterator>(
            const_cast<std::unordered_set<T> &>(_container).begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<HashSet<T>::HashSetConstIterator>(
            const_cast<std::unordered_set<T> &>(_container).end());
    }

    ref<Set<T>> copy() const override
    {
        auto other = Object::create<HashSet<T>>();
        other->_container = _container;
        return other;
    }

    ref<ConstIterator<T>> find(const T &key) const override
    {
        return Object::create<HashSet<T>::HashSetConstIterator>(
            const_cast<std::unordered_set<T> &>(_container).find(key));
    }

    ref<ConstIterator<T>> erase(ref<ConstIterator<T>> iter) override
    {
        auto iterator = iter->template covariant<HashSet<T>::HashSetConstIterator>();
        return Object::create<HashSet<T>::HashSetConstIterator>(_container.erase(iterator->_it));
    }

    size_t size() const override { return _container.size(); }

    void emplace(const T &v) override { _container.insert(v); }
    void emplace(T &&v) override { _container.insert(std::move(v)); }

    void clear() override { _container.clear(); }
};

template <typename T>
bool HashSet<T>::HashSetConstIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<HashSet<T>::HashSetConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    return false;
}
