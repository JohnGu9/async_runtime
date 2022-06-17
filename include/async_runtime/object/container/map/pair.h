#pragma once

#include "../map.h"
/**
 * @brief Pair for Map element type.
 *
 * @tparam First the [Key] type
 * @tparam Second the [Value] type
 */
template <typename First, typename Second>
class Pair : public Object
{
public:
    First first;
    Second second;

    Pair(const First &first) : first(first), second() {}
    Pair(First &&first) : first(std::move(first)), second() {}

    Pair(const First &first, const Second &second)
        : first(first), second(second) {}
    Pair(First &&first, Second &&second)
        : first(std::move(first)), second(std::move(second)) {}

    Pair(const std::pair<First, Second> &pair)
        : first(pair.first), second(pair.second) {}
    Pair(std::pair<First, Second> &&pair)
        : first(std::move(pair.first)), second(std::move(pair.second)) {}

    bool operator==(ref<Object> other)
    {
        if (auto ptr = dynamic_cast<Pair<First, Second> *>(other.get()))
        {
            return ptr->first == this->first && ptr->second == this->second;
        }
        return false;
    }
};

template <typename First, typename Second>
class ref<Pair<First, Second>> : public _async_runtime::RefImplement<Pair<First, Second>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Pair<First, Second>>;
    using element_type = Pair<First, Second>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(const First &first) : super(std::make_shared<element_type>(first)) {}
    ref(First &&first) : super(std::make_shared<element_type>(std::move(first))) {}

    ref(const First &first, const Second &second) : super(std::make_shared<element_type>(first, second)) {}
    ref(First &&first, Second &&second) : super(std::make_shared<element_type>(std::move(first), std::move(second))) {}

    ref(const std::pair<First, Second> &pair) : super(std::make_shared<element_type>(pair)) {}
    ref(std::pair<First, Second> &&pair) : super(std::make_shared<element_type>(std::move(pair))) {}

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
};

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
            return std::hash<Key>()(pair->first);
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
