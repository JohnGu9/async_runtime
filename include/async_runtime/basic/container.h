#pragma once

#include "function.h"

template <typename Element = std::nullptr_t>
class Container;

template <>
class Container<std::nullptr_t>
{
};

template <typename Element>
class Container : public Container<std::nullptr_t>
{
    virtual void forEach(Function<void(const Element &)>) const = 0;
};

template <typename Key, typename Value>
class IndexableContainer : public Container<Value>
{
    virtual const Value &operator[](const Key &key) const = 0;
    virtual Value &operator[](const Key &key) = 0;

    virtual const Value &operator[](Key &&key) const = 0;
    virtual Value &operator[](Key &&key) = 0;
};