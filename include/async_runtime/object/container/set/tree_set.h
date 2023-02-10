#pragma once
#include "std_implement.h"
#include <set>

template <typename T, typename Compare = std::less<T>>
class TreeSet : public StdSetImplement<T, std::set<T, Compare>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = StdSetImplement<T, std::set<T, Compare>>;

public:
    using super::super;
    ref<Set<T>> copy() const override
    {
        auto other = Object::create<TreeSet<T>>();
        other->_container = this->_container;
        return other;
    }
};
