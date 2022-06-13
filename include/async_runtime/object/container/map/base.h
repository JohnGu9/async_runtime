#pragma once

#include "../map.h"
#include <map>

template <typename Key, typename Value>
class Map : public MutableIterable<typename std::map<Key, Value>::value_type>,
            public IndexableMixin<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    Map() {}

public:
    using T = typename std::map<Key, Value>::value_type; // std::pair<Key, Value>
    static ref<Map<Key, Value>> create();

    virtual ref<Map<Key, Value>> copy() const = 0;
    virtual ref<Iterable<Key>> keys() const;
    virtual ref<Iterable<Value>> values() const;

    template <typename R>
    ref<Map<Key, R>> map(Function<R(const Value &)>) const;

    virtual ref<Iterator<T>> find(const Key &) = 0;
    virtual ref<Iterator<T>> find(Key &&key) { return this->find(static_cast<const Key &>(key)); }

    virtual ref<ConstIterator<T>> find(const Key &) const = 0;
    virtual ref<ConstIterator<T>> find(Key &&key) const { return this->find(static_cast<const Key &>(key)); }

    virtual bool remove(const Key &) = 0;
    virtual bool remove(Key &&v) { return this->remove(static_cast<const Key &>(v)); }

    virtual bool contain(const Key &other) const { return this->find(other) != this->end(); }
    virtual bool contain(Key &&other) const { return this->find(std::move(other)) != this->end(); }

    bool contain(const T &other) const
    {
        auto it = this->find(other.first);
        if (it == this->end())
            return false;
        return (*it).second == other.second;
    }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(Key).name() << ", " << typeid(Value).name() << ">{ ";
        for (const T &element : *this)
            os << '{' << element.first << " : " << element.second << "}, ";
        os << "}";
    }
};
