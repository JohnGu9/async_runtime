#pragma once

#include "../container.h"

template <>
class List<std::nullptr_t> : virtual public Container<std::nullptr_t>
{
};

template <typename Element>
class List : public List<std::nullptr_t>, public IndexableContainer<size_t, Element>
{
};