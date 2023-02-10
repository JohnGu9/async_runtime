#pragma once
#include "base.h"

template <typename T, typename Container>
class StdListImplement : public List<T>
{
protected:
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    Container _container;
    using StdIterator = typename Container::iterator;
    using StdConstIterator = typename Container::const_iterator;
    virtual StdIterator _iterAt(size_t index) = 0;

public:
    class _ConstIterator : public ConstIterator<T>
    {
        using iterator = typename Container::const_iterator;

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
        using iterator = typename Container::iterator;

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

    StdListImplement() = default;
    StdListImplement(std::initializer_list<T> list) : _container(std::move(list)) {}

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<_ConstIterator>(this->_container.begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<_ConstIterator>(this->_container.end());
    }

    ref<Iterator<T>> begin() override
    {
        return Object::create<_Iterator>(this->_container.begin());
    }

    ref<Iterator<T>> end() override
    {
        return Object::create<_Iterator>(this->_container.end());
    }

    ref<Iterator<T>> erase(ref<Iterator<T>> iter) override
    {
        auto iterator = iter.get()->template covariant<_Iterator>();
        return Object::create<_Iterator>(_container.erase(iterator.get()->_it));
    }

    T &operator[](const size_t &index) override { return *(this->_iterAt(index)); }

    size_t size() const override { return this->_container.size(); }

    void emplace(const T &v) override { this->_container.emplace_back(v); }
    void emplace(T &&v) override { this->_container.emplace_back(std::move(v)); }

    void removeAt(size_t index) override
    {
        auto iter = _iterAt(index);
        this->_container.erase(iter);
    }

    bool insertAt(size_t index, const T &value) override
    {
        auto start = this->_iterAt(index);
        this->_container.insert(start, value);
        return true;
    }

    bool insertAt(size_t index, T &&value) override
    {
        auto start = this->_iterAt(index);
        this->_container.insert(start, std::move(value));
        return true;
    }

    void clear() override { this->_container.clear(); }
};

template <typename T, typename Container>
bool StdListImplement<T, Container>::_ConstIterator::operator==(const option<Object> &other)
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

template <typename T, typename Container>
bool StdListImplement<T, Container>::_Iterator::operator==(const option<Object> &other)
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
