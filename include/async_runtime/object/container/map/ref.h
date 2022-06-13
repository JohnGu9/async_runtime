#pragma once

#include "../map.h"

template <typename Key, typename Value>
class ref<Map<Key, Value>> : public _async_runtime::RefImplement<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Map<Key, Value>>;

public:
    using value_type = typename Map<Key, Value>::T;

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(const std::initializer_list<value_type> &list) : super(std::make_shared<HashMap<Key, Value>>(list)) {}
    ref(std::initializer_list<value_type> &&list) : super(std::make_shared<HashMap<Key, Value>>(std::move(list))) {}

    template <typename... Args>
    Value &operator[](Args &&...key) const { return (*this)->operator[](std::forward<Args>(key)...); }

    ref<Iterator<value_type>> begin() const { return (*this)->begin(); }
    ref<Iterator<value_type>> end() const { return (*this)->end(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
};

template <typename Key, typename Value>
ref<Map<Key, Value>> Map<Key, Value>::create() { return Object::create<HashMap<Key, Value>>(); }

template <typename Key, typename Value>
template <typename R>
ref<Map<Key, R>> Map<Key, Value>::map(Function<R(const Value &)> fn) const
{
    auto mapped = HashMap<Key, R>::create();
    for (const auto &pair : *this)
    {
        mapped[pair.first] = fn(pair.second);
    }
    return mapped;
}

template <typename Key, typename Value>
class lateref<Map<Key, Value>> : public ref<Map<Key, Value>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<Map<Key, Value>>;
    using value_type = typename super::value_type;

public:
    explicit lateref() : super() {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Map<Key, Value>, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : super(std::move(other)) {}

    lateref(const std::initializer_list<value_type> &list) : super(list) {}
    lateref(std::initializer_list<value_type> &&list) : super(std::move(list)) {}
};
