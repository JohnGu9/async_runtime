#pragma once

#include "../map.h"
#include "../set.h"
#include "pair.h"
#include <map>

template <class F, class S>
std::ostream &operator<<(std::ostream &os, const std::pair<F, S> &out)
{
    std::lock_guard<std::recursive_mutex> lk(_async_runtime::OstreamStack::mutex);
    ++_async_runtime::OstreamStack::depth;
    os << '{' << out.first << " : " << out.second << '}';
    --_async_runtime::OstreamStack::depth;
    return os;
}

namespace _async_runtime
{
    template <typename T>
    class KeyHasher;

    template <typename Key, typename Value>
    class KeyHasher<ref<Pair<const Key, Value>>>
    {
    public:
        using T = ref<Pair<const Key, Value>>;
        size_t operator()(const T &pair) const noexcept
        {
            return std::hash<Key>()(pair->first); // map -0 to 0
        }
    };
    template <typename T>
    class KeyEqual;

    template <typename Key, typename Value>
    class KeyEqual<ref<Pair<const Key, Value>>>
    {
    public:
        using T = ref<Pair<const Key, Value>>;
        constexpr bool operator()(const T &left, const T &right) const
        {
            return std::equal_to<Key>()(left->first, right->first);
        }
    };
};

template <typename Key, typename Value>
class Map : public Iterable<ref<Pair<const Key, Value>>>,
            public IndexableMixin<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    Map() {}

public:
    using T = ref<Pair<const Key, Value>>;
    using std_pair = std::pair<const Key, Value>;

    static ref<Map<Key, Value>> create();
    static ref<Map<Key, Value>> merge(std::initializer_list<Map<Key, Value>>);

    virtual ref<Map<Key, Value>> copy() const = 0;
    virtual ref<Iterable<Key>> keys() const;
    virtual ref<Iterable<Value>> values() const;

    template <typename R>
    ref<Map<Key, R>> map(Function<R(const T &)>) const;

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

    virtual bool insert(const Key &key, const Value &value) { return this->emplace(key, value); }
    virtual bool insert(Key &&key, Value &&value) { return this->emplace(std::move(key), std::move(value)); }

    virtual void merge(ref<Map<Key, Value>> other);

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(Key).name() << ", " << typeid(Value).name() << ">{ ";
        for (const T &element : *this)
            os << element << ", ";
        os << "}";
    }
};
