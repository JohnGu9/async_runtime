#pragma once

#include "../map.h"
#include <map>

template <typename Key, typename Value>
class Map : public Iterable<std::pair<const Key, Value>>,
            public IndexableMixin<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    Map() {}

public:
    using T = std::pair<const Key, Value>;
    static ref<Map<Key, Value>> create();

    virtual ref<Map<Key, Value>> copy() const = 0;
    virtual ref<Iterable<Key>> keys() const;
    virtual ref<Iterable<Value>> values() const;

    template <typename R>
    ref<Map<Key, R>> map(Function<R(const Value &)>) const;

    virtual ref<ConstIterator<T>> findKey(const Key &) const = 0;
    virtual ref<ConstIterator<T>> findKey(Key &&key) const { return this->findKey(static_cast<const Key &>(key)); }

    virtual bool containsKey(const Key &other) const { return this->findKey(other) != this->end(); }
    virtual bool containsKey(Key &&other) const { return this->containsKey(static_cast<const Key &>(other)); }

    virtual bool removeKey(const Key &) = 0;
    virtual bool removeKey(Key &&v) { return this->removeKey(static_cast<const Key &>(v)); }

    virtual ref<ConstIterator<T>> erase(ref<ConstIterator<T>> iter) = 0;
    virtual void clear() = 0;

    virtual bool emplace(const Key &key, const Value &value) = 0;
    virtual bool emplace(Key &&key, Value &&value) = 0;

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(Key).name() << ", " << typeid(Value).name() << ">{ ";
        for (const T &element : *this)
            os << '{' << element.first << " : " << element.second << "}, ";
        os << "}";
    }
};
