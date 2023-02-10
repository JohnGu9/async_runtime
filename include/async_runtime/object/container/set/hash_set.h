#pragma once
#include "std_implement.h"
#include <unordered_set>

template <typename T, typename Hasher = std::hash<T>, typename Equal = std::equal_to<T>>
class HashSet : public StdSetImplement<T, std::unordered_set<T, Hasher, Equal>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = StdSetImplement<T, std::unordered_set<T, Hasher, Equal>>;

public:
    using super::super;
    ref<Set<T>> copy() const override
    {
        auto other = Object::create<HashSet<T>>();
        other->_container = this->_container;
        return other;
    }
};
