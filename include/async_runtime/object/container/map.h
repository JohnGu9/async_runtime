#pragma once

#include "../container.h"
#include <initializer_list>
#include <unordered_map>

template <typename Key, typename Value>
class Map : public std::unordered_map<Key, Value>,
            public Iterable<typename std::unordered_map<Key, Value>::value_type>,
            public RemovableMixin<Key>,
            public virtual ContainableMixin<Key>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    using iterator = typename std::unordered_map<Key, Value>::iterator;
    using const_iterator = typename std::unordered_map<Key, Value>::const_iterator;
    using value_type = typename std::unordered_map<Key, Value>::value_type;
    static ref<Map<Key, Value>> create();

    Map() {}
    Map(const Map<Key, Value> &other) : std::unordered_map<Key, Value>(other) {}

    Map(std::initializer_list<value_type> &&list) : std::unordered_map<Key, Value>(std::move(list)) {}
    Map(const std::initializer_list<value_type> &list) : std::unordered_map<Key, Value>(list) {}
    template <typename R>
    Map(const std::initializer_list<R> &list) : std::unordered_map<Key, Value>(list.begin(), list.end()) {}

    ref<Map<Key, Value>> copy() const;
    template <typename R>
    ref<Map<Key, R>> map(Function<R(const Value &)>) const;

    bool any(Function<bool(const value_type &)> fn) const override
    {
        for (const auto &iter : *this)
            if (fn(iter))
                return true;
        return false;
    }

    bool every(Function<bool(const value_type &)> fn) const override
    {
        for (const auto &iter : *this)
            if (!fn(iter))
                return false;
        return true;
    }

    void forEach(Function<void(const value_type &)> fn) const override
    {
        for (const auto &iter : *this)
            fn(iter);
    }

    bool remove(const Key &v) override
    {
        auto iter = this->find(v);
        if (iter == this->end())
            return false;
        this->erase(iter);
        return true;
    }

    bool contain(const Key &other) const override
    {
        return this->find(other) != this->end();
    }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(Key).name() << ", " << typeid(Value).name() << ">{ ";
        for (const std::pair<Key, Value> &element : *this)
            os << '{' << element.first << " : " << element.second << "}, ";
        os << "}";
    }
};

template <typename Key, typename Value>
class ref<Map<Key, Value>> : public _async_runtime::RefImplement<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Map<Key, Value>>;

public:
    using iterator = typename Map<Key, Value>::iterator;
    using const_iterator = typename Map<Key, Value>::const_iterator;
    using value_type = typename Map<Key, Value>::value_type;

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(const std::initializer_list<value_type> &list) : super(std::make_shared<Map<Key, Value>>(list)) {}
    ref(std::initializer_list<value_type> &&list) : super(std::make_shared<Map<Key, Value>>(std::move(list))) {}

    template <typename... Args>
    Value &operator[](Args &&...key) const { return (*this)->operator[](std::forward<Args>(key)...); }

    iterator begin()
    {
        return (*this)->begin();
    }

    const_iterator begin() const
    {
        return (*this)->begin();
    }

    iterator end()
    {
        return (*this)->end();
    }
    const_iterator end() const
    {
        return (*this)->end();
    }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<Map<Key, Value>>(other) {}
};

template <typename Key, typename Value>
ref<Map<Key, Value>> Map<Key, Value>::create()
{
    return Object::create<Map<Key, Value>>();
}

template <typename Key, typename Value>
ref<Map<Key, Value>> Map<Key, Value>::copy() const
{
    return Object::create<Map<Key, Value>>(*this);
}

template <typename Key, typename Value>
template <typename R>
ref<Map<Key, R>> Map<Key, Value>::map(Function<R(const Value &)> fn) const
{
    auto mapped = Object::create<Map<Key, R>>();
    for (const auto &pair : *this)
        mapped[pair.first] = fn(pair.second);
    return mapped;
}

template <typename Key, typename Value>
class lateref<Map<Key, Value>> : public ref<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<Map<Key, Value>>;
    using iterator = typename super::iterator;
    using const_iterator = typename super::const_iterator;
    using value_type = typename super::value_type;

public:
    explicit lateref() : ref<Map<Key, Value>>() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<Map<Key, Value>>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : ref<Map<Key, Value>>(std::move(other)) {}

    lateref(const std::initializer_list<value_type> &list) : ref<Map<Key, Value>>(list) {}
    lateref(std::initializer_list<value_type> &&list) : ref<Map<Key, Value>>(std::move(list)) {}
};
