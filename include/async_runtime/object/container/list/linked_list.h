#pragma once
#include "base.h"
#include <list>

template <typename T>
class LinkedList : public StdListImplement<T, std::list<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = StdListImplement<T, std::list<T>>;

public:
    using super::super;
    ref<List<T>> copy() const override
    {
        auto other = Object::create<LinkedList<T>>();
        other->_container = this->_container;
        return other;
    }

protected:
    typename super::StdIterator _iterAt(size_t index) override
    {
        auto b = this->_container.begin();
        for (size_t i = 0; i < index; i++)
            ++b;
        return b;
    }
};
