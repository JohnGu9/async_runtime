#pragma once
#include "base.h"
#include <vector>

template <typename T>
class VectorList : public StdListImplement<T, std::vector<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = StdListImplement<T, std::vector<T>>;

public:
    using super::super;
    ref<List<T>> copy() const override
    {
        auto other = Object::create<VectorList<T>>();
        other->_container = this->_container;
        return other;
    }

protected:
    typename super::StdIterator _iterAt(size_t index) override
    {
        return this->_container.begin() + index;
    }
};
