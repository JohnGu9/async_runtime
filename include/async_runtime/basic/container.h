#pragma once

#include "function.h"

template <typename Element = std::nullptr_t>
class Container;

template <typename Element = std::nullptr_t>
class List;

template <typename Key, typename Value>
class Map;

template <typename Element = std::nullptr_t>
class Set;

template <>
class Container<std::nullptr_t>
{
public:
    virtual bool isEmpty() const = 0;
    virtual bool isNotEmpty() const { return !this->isEmpty(); }
};

template <typename Element>
class Container : virtual public Container<std::nullptr_t>
{
public:
    virtual void forEach(Function<void(const Element &)>) = 0;
    virtual bool containe(Element element) const = 0;
    virtual bool any(Function<bool(const Element &)>) = 0;
    virtual bool every(Function<bool(const Element &)>) = 0;
    virtual void remove(Element) = 0;

    virtual List<Element> toList() = 0;
    virtual Set<Element> toSet() = 0;

    // virtual Container<Element> takeWhile(Function<bool(const Element &)>) = 0;   // not change origin container
    // virtual Container<Element> removeWhile(Function<bool(const Element &)>) = 0; // remove all select element and put it to new container
};

template <typename Key, typename Value>
class IndexableContainer : public Container<Value>
{
public:
    virtual const Value &operator[](const Key &key) const = 0;
    virtual Value &operator[](const Key &key) = 0;
    virtual const Value &operator[](Key &&key) const = 0;
    virtual Value &operator[](Key &&key) = 0;
};

#include "container/list.h"
#include "container/map.h"
#include "container/set.h"
