#pragma once

#include "../container.h"

template <>
class Set<std::nullptr_t> : virtual public Container<std::nullptr_t>
{
};

template <typename Element>
class Set : public Set<std::nullptr_t>, public IndexableContainer<size_t, Element>
{
};