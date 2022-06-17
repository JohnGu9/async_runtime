#pragma once
#include "base.h"
#include <unordered_set>

template <typename T, typename Hasher = std::hash<T>, typename Equal = std::equal_to<T>>
class HashSet : public Set<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using container_type = std::unordered_set<T, Hasher, Equal>;
    container_type _container;

    container_type &container() const
    {
        return const_cast<container_type &>(_container);
    }

public:
    class HashSetConstIterator : public ConstIterator<T>
    {
        using iterator = typename container_type::iterator;

    public:
        const iterator _it;
        HashSetConstIterator(const iterator it) : _it(it) {}

        ref<ConstIterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<HashSetConstIterator>(std::move(++copy));
        }

        const T &value() const override { return *(this->_it); }

        bool operator==(ref<Object> other) override;
    };

    static ref<Set<T>> create();

    HashSet() : _container{} {}
    HashSet(std::initializer_list<T> &&list) : _container(std::move(list)) {}
    HashSet(const std::initializer_list<T> &list) : _container(list) {}

    ref<ConstIterator<T>> begin() const override
    {
        return Object::create<HashSetConstIterator>(
            container().begin());
    }

    ref<ConstIterator<T>> end() const override
    {
        return Object::create<HashSetConstIterator>(
            container().end());
    }

    ref<Set<T>> copy() const override
    {
        auto other = Object::create<HashSet<T>>();
        other->_container = _container;
        return other;
    }

    ref<ConstIterator<T>> find(const T &key) const override
    {
        return Object::create<HashSetConstIterator>(
            container().find(key));
    }

    ref<ConstIterator<T>> erase(ref<ConstIterator<T>> iter) override
    {
        auto iterator = iter.get()->template covariant<HashSetConstIterator>();
        return Object::create<HashSetConstIterator>(_container.erase(iterator.get()->_it));
    }

    size_t size() const override { return _container.size(); }

    void emplace(const T &v) override { _container.insert(v); }
    void emplace(T &&v) override { _container.insert(std::move(v)); }

    void clear() override { _container.clear(); }
};

template <typename T, typename Hasher, typename Equal>
bool HashSet<T, Hasher, Equal>::HashSetConstIterator::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<typename HashSet<T, Hasher, Equal>::HashSetConstIterator *>(other.get())) // [[likely]]
    {
        return this->_it == ptr->_it;
    }
    return false;
}
