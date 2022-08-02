#pragma once
#include "../map.h"
#include "../pair.h"
#include "../set.h"
#include <map>

template <typename Key, typename Value>
class Map : public Iterable<ref<Pair<const Key, Value>>>,
            public IndexableMixin<Key, Value>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using iterable_type = ref<Iterable<ref<Pair<const Key, Value>>>>;

public:
    using element_type = ref<Pair<const Key, Value>>;

    static ref<Map<Key, Value>> create();
    static ref<Map<Key, Value>> merge(ref<Iterable<iterable_type>>);

    virtual ref<Map<Key, Value>> copy() const = 0;
    virtual ref<Iterable<Key>> keys() const;
    virtual ref<Iterable<Value>> values() const;

    virtual ref<ConstIterator<element_type>> findKey(const Key &) const = 0;
    virtual ref<ConstIterator<element_type>> findKey(Key &&key) const { return this->findKey(static_cast<const Key &>(key)); }

    virtual bool containsKey(const Key &other) const { return this->findKey(other) != this->end(); }
    virtual bool containsKey(Key &&other) const { return this->containsKey(static_cast<const Key &>(other)); }

    virtual bool removeKey(const Key &) = 0;
    virtual bool removeKey(Key &&v) { return this->removeKey(static_cast<const Key &>(v)); }

    virtual ref<ConstIterator<element_type>> erase(ref<ConstIterator<element_type>> iter) = 0;
    virtual void clear() = 0;

    virtual bool emplace(const Key &key, const Value &value) = 0;
    virtual bool emplace(Key &&key, Value &&value) = 0;

    virtual bool insert(const Key &key, const Value &value) { return this->emplace(key, value); }
    virtual bool insert(Key &&key, Value &&value) { return this->emplace(std::move(key), std::move(value)); }

    virtual void merge(iterable_type other);

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(Key).name() << ", " << typeid(Value).name() << ">{ ";
        for (const element_type &element : *this)
            os << element << ", ";
        os << "}";
    }

    template <typename R>
    ref<Map<Key, R>> map(Function<R(const element_type &)>) const;

protected:
    Map() noexcept = default;
    element_type keyOnlyPairBuilder(const Key &key) const { return element_type(key); }
    element_type keyOnlyPairBuilder(Key &&key) const { return element_type(std::move(key)); }
};
