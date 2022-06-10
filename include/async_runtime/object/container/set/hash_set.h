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
        using iterator = typename std::unordered_set<T>::const_iterator;

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

    class HashSetIterator : public Iterator<T>
    {
        using iterator = typename std::unordered_set<T>::iterator;

    public:
        const iterator _it;
        HashSetIterator(const iterator it) : _it(it) {}

        ref<Iterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<HashSet<T>::HashSetIterator>(std::move(++copy));
        }

        T &value() const override { return const_cast<T &>(*(this->_it)); }

        bool operator==(ref<Object> other) override;

        ref<ConstIterator<T>> toConst() const override
        {
            return Object::create<HashSet<T>::HashSetConstIterator>(_it);
        }
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

    ref<Set<T>> copy() const override
    {
        auto other = Object::create<HashSet<T>>();
        other->_container = _container;
        return other;
    }

    ref<Iterator<T>> find(const T &key) override
    {
        return Object::create<HashSet<T>::HashSetIterator>(_container.find(key));
    }

    ref<ConstIterator<T>> find(const T &key) const override
    {
        return Object::create<HashSet<T>::HashSetConstIterator>(_container.find(key));
    }

    ref<Iterator<T>> erase(ref<Iterator<T>> iter) override
    {
        auto iterator = iter->template covariant<HashSet<T>::HashSetIterator>();
        return Object::create<HashSet<T>::HashSetIterator>(_container.erase(iterator->_it));
    }

    ref<Iterator<T>> begin() override
    {
        return Object::create<HashSet<T>::HashSetIterator>(_container.begin());
    }

    ref<Iterator<T>> end() override
    {
        return Object::create<HashSet<T>::HashSetIterator>(_container.end());
    }

    size_t size() const override { return _container.size(); }

    bool contain(const T &other) const override
    {
        return _container.find(other) != _container.end();
    }

    bool add(const T &v) override
    {
        _container.insert(v);
        return true;
    }

    bool add(T &&v) override
    {
        _container.insert(std::move(v));
        return true;
    }

    bool remove(const T &v) override
    {
        auto iter = _container.find(v);
        if (iter == _container.end())
            return false;
        _container.erase(iter);
        return true;
    }

    bool removeAll(const T &value) override
    {
        return this->remove(value);
    }

    void clear() override { _container.clear(); }
};

template <typename T>
bool HashSet<T>::HashSetConstIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<HashSet<T>::HashSetIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<HashSet<T>::HashSetConstIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}

template <typename T>
bool HashSet<T>::HashSetIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<HashSet<T>::HashSetConstIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<HashSet<T>::HashSetIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}
