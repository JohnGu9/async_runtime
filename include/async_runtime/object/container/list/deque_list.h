#pragma once
#include "std_implement.h"
#include <deque>

template <typename T>
class DequeList : public StdListImplement<T, std::deque<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = StdListImplement<T, std::deque<T>>;

public:
    using super::super;
    ref<List<T>> copy() const override
    {
        auto other = Object::create<DequeList<T>>();
        other->_container = this->_container;
        return other;
    }

protected:
    typename super::StdIterator _iterAt(size_t index) override
    {
        return this->_container.begin() + index;
    }
};
