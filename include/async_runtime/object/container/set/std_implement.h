#pragma once
#include "base.h"

template <typename T, typename Container>
class StdSetImplement : public Set<T>
{
protected:
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    Container _container;
    using StdIterator = typename Container::iterator;
    using StdConstIterator = typename Container::const_iterator;

public:
    class _ConstIterator : public ConstIterator<T>
    {
        using iterator = typename Container::iterator;

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

    StdSetImplement() = default;
    StdSetImplement(std::initializer_list<T> list) : _container(std::move(list)) {}

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<_ConstIterator>(_container.begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<_ConstIterator>(_container.end());
    }

    ref<ConstIterator<T>> find(const T &key) const override
    {
        return Object::create<_ConstIterator>(
            _container.find(key));
    }

    ref<ConstIterator<T>> erase(ref<ConstIterator<T>> iter) override
    {
        auto iterator = iter.get()->template covariant<_ConstIterator>();
        return Object::create<_ConstIterator>(_container.erase(iterator.get()->_it));
    }

    size_t size() const override { return _container.size(); }

    void emplace(const T &v) override { _container.insert(v); }
    void emplace(T &&v) override { _container.insert(std::move(v)); }

    void clear() override { _container.clear(); }
};

template <typename T, typename Container>
bool StdSetImplement<T, Container>::_ConstIterator::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<_ConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    return false;
}
