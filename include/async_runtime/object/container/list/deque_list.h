#pragma once
#include "base.h"
#include <deque>

template <typename T>
class DequeList : public List<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    std::deque<T> _container;

public:
    class DequeListConstIterator : public ConstIterator<T>
    {
        using iterator = typename std::deque<T>::const_iterator;

    public:
        const iterator _it;
        DequeListConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<DequeList<T>::DequeListConstIterator>(std::move(++copy));
        }

        const T &value() const override { return *(this->_it); }

        bool operator==(ref<Object> other) override;
    };

    class DequeListIterator : public Iterator<T>
    {
        using iterator = typename std::deque<T>::iterator;

    public:
        const iterator _it;
        DequeListIterator(const iterator it) : _it(it) {}

        ref<Iterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<DequeList<T>::DequeListIterator>(std::move(++copy));
        }

        T &value() const override { return *(this->_it); }

        bool operator==(ref<Object> other) override;

        ref<ConstIterator<T>> toConst() const override
        {
            return Object::create<DequeList<T>::DequeListConstIterator>(_it);
        }
    };

    static ref<List<T>> create();

    DequeList() {}
    DequeList(const std::initializer_list<T> &list) : _container(list) {}
    DequeList(std::initializer_list<T> &&list) : _container(std::move(list)) {}

    ref<List<T>> copy() const override
    {
        auto other = Object::create<DequeList<T>>();
        other->_container = _container;
        return other;
    }

    ref<Iterator<T>> begin() override
    {
        return Object::create<DequeList<T>::DequeListIterator>(_container.begin());
    }

    ref<Iterator<T>> end() override
    {
        return Object::create<DequeList<T>::DequeListIterator>(_container.end());
    }

    ref<Iterator<T>> erase(ref<Iterator<T>> iter) override
    {
        auto iterator = iter->template covariant<DequeList<T>::DequeListIterator>();
        return Object::create<DequeList<T>::DequeListIterator>(_container.erase(iterator->_it));
    }

    T &operator[](const size_t &index) override { return _container[index]; }

    size_t size() const override { return _container.size(); }

    bool add(const T &v) override
    {
        _container.emplace_back(v);
        return true;
    }

    bool add(T &&v) override
    {
        _container.emplace_back(std::move(v));
        return true;
    }

    void removeAt(const size_t index) override
    {
        RUNTIME_ASSERT(index < this->size(), "Index overflow");
        auto iter = _container.begin() + index;
        _container.erase(iter);
    }

    void clear() override { _container.clear(); }
};

template <typename T>
bool DequeList<T>::DequeListConstIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<DequeList<T>::DequeListConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<DequeList<T>::DequeListIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}

template <typename T>
bool DequeList<T>::DequeListIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<DequeList<T>::DequeListIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    else if (auto ptr = dynamic_cast<DequeList<T>::DequeListConstIterator *>(other.get()))
    {
        return this->_it == ptr->_it;
    }
    return false;
}