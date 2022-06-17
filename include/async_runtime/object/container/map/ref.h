#pragma once

#include "../map.h"

#ifndef ASYNC_RUNTIME_CUSTOM_MAP_CONSTRUCTOR
namespace _async_runtime
{
    template <typename Key, typename Value>
    using DefaultMap = HashMap<Key, Value>;
};
#else
ASYNC_RUNTIME_CUSTOM_MAP_CONSTRUCTOR
#endif

template <typename Key, typename Value>
class ref<Map<Key, Value>> : public _async_runtime::RefImplement<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Map<Key, Value>>;

public:
    using value_type = typename Map<Key, Value>::T;
    using init_type = typename Map<Key, Value>::std_pair;

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(const std::initializer_list<init_type> &list) : super(Object::create<_async_runtime::DefaultMap<Key, Value>>(list)) {}
    ref(std::initializer_list<init_type> &&list) : super(Object::create<_async_runtime::DefaultMap<Key, Value>>(std::move(list))) {}

    template <typename... Args>
    Value &operator[](Args &&...key) const { return (*this)->operator[](std::forward<Args>(key)...); }

    ref<ConstIterator<value_type>> begin() const { return (*this)->begin(); }
    ref<ConstIterator<value_type>> end() const { return (*this)->end(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(std::shared_ptr<R> &&other) : super(std::move(other)) {}
};

template <typename Key, typename Value>
class lateref<Map<Key, Value>> : public ref<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<Map<Key, Value>>;

public:
    using value_type = typename super::value_type;
    using init_type = typename Map<Key, Value>::std_pair;
    explicit lateref() : super() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : super(std::move(other)) {}

    lateref(const std::initializer_list<init_type> &list) : super(list) {}
    lateref(std::initializer_list<init_type> &&list) : super(std::move(list)) {}
};

template <typename Key, typename Value>
ref<Map<Key, Value>> Map<Key, Value>::create() { return Object::create<_async_runtime::DefaultMap<Key, Value>>(); }

template <typename Key, typename Value>
template <typename R>
ref<Map<Key, R>> Map<Key, Value>::map(Function<R(const T &)> fn) const
{
    auto mapped = HashMap<Key, R>::create();
    for (const auto &pair : *this)
    {
        auto key = pair->first;
        mapped->emplace(std::move(key), fn(pair));
    }
    return mapped;
}

template <typename Key, typename Value>
void Map<Key, Value>::merge(iterable_type other)
{
    for (const T &pair : other)
        this->emplace(pair->first, pair->second);
}

template <typename Key, typename Value>
ref<Map<Key, Value>> Map<Key, Value>::merge(ref<List<iterable_type>> list)
{
    auto map = Map<Key, Value>::create();
    for (const iterable_type &element : list)
        map->merge(element);
    return map;
}
