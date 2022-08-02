#pragma once
#include "base.h"
#include <deque>

template <typename T>
class DequeList : public List<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    std::deque<T> _container;

public:
    class _ConstIterator : public ConstIterator<T>
    {
        using iterator = typename std::deque<T>::const_iterator;

    public:
        const iterator _it;
        _ConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<_ConstIterator>(std::move(++copy));
        }

        const T &value() const override { return *(this->_it); }

        bool operator==(const option<Object> &other) override;
    };

    class _Iterator : public Iterator<T>
    {
        using iterator = typename std::deque<T>::iterator;

    public:
        const iterator _it;
        _Iterator(const iterator it) : _it(it) {}

        ref<Iterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<_Iterator>(std::move(++copy));
        }

        T &value() const override { return *(this->_it); }

        bool operator==(const option<Object> &other) override;

        ref<ConstIterator<T>> toConst() const override
        {
            return Object::create<_ConstIterator>(_it);
        }
    };

    static ref<List<T>> create();

    DequeList() {}
    DequeList(std::initializer_list<T> list) : _container(std::move(list)) {}

    ref<List<T>> copy() const override
    {
        auto other = Object::create<DequeList<T>>();
        other->_container = _container;
        return other;
    }

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<_ConstIterator>(_container.begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<_ConstIterator>(_container.end());
    }

    ref<Iterator<T>> begin() override
    {
        return Object::create<_Iterator>(_container.begin());
    }

    ref<Iterator<T>> end() override
    {
        return Object::create<_Iterator>(_container.end());
    }

    ref<Iterator<T>> erase(ref<Iterator<T>> iter) override
    {
        auto iterator = iter.get()->template covariant<_Iterator>();
        return Object::create<_Iterator>(_container.erase(iterator.get()->_it));
    }

    T &operator[](const size_t &index) override { return _container[index]; }

    size_t size() const override { return _container.size(); }

    void emplace(const T &v) override { _container.emplace_back(v); }
    void emplace(T &&v) override { _container.emplace_back(std::move(v)); }

    void removeAt(size_t index) override
    {
        RUNTIME_ASSERT(index < this->size(), "Index overflow");
        auto iter = _container.begin() + index;
        _container.erase(iter);
    }

    bool insertAt(size_t index, const T &value) override
    {
        auto start = _container.begin() + index;
        _container.insert(start, value);
        return true;
    }

    bool insertAt(size_t index, T &&value) override
    {
        auto start = _container.begin() + index;
        _container.insert(start, std::move(value));
        return true;
    }

    void clear() override { _container.clear(); }
};

template <typename T>
bool DequeList<T>::_ConstIterator::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<_ConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<_Iterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}

template <typename T>
bool DequeList<T>::_Iterator::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<_Iterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<_ConstIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}
