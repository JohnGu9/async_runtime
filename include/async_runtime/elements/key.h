#pragma once

#include <list>
#include "element.h"

class Widget;

class Key : public Object
{
public:
    virtual bool equal(option<Key> other) = 0;
    virtual void setElement(ref<Element> element);
    virtual void dispose();
    virtual option<const Widget> getCurrentWidget();

protected:
    virtual option<Element> getElement();

private:
    weakref<Element> _element;
};

template <typename T>
class ValueKey : public Key
{
public:
    ValueKey(T &value) : _value(value) {}
    virtual bool equal(option<Key> other) override
    {
        lateref<Key> nonNullOther;
        if (other.isNotNull(nonNullOther))
        {
            lateref<ValueKey<T>> castedPointer;
            if (nonNullOther->cast<ValueKey<T>>().isNotNull(castedPointer))
            {
                return castedPointer->_value == this->_value;
            }
        }
        return false;
    }

protected:
    T _value;
};

class GlobalKey : public Key
{
public:
    inline virtual bool equal(option<Key> other)
    {
        return Object::identical(this->shared_from_this(), other);
    }

    virtual option<BuildContext> getCurrentContext();
    virtual option<State<StatefulWidget>> getCurrentState();
};

class GlobalObjectKey : public GlobalKey
{
public:
    GlobalObjectKey(option<Object> object) : _object(object){};
    virtual bool equal(option<Key> other)
    {
        lateref<Key> nonNullOther;
        if (other.isNotNull(nonNullOther))
        {
            lateref<GlobalObjectKey> castedPointer;
            if (nonNullOther->cast<GlobalObjectKey>().isNotNull(castedPointer))
                return Object::identical(this->_object, castedPointer->_object);
        }
        return false;
    }

protected:
    GlobalObjectKey() = delete;
    option<Object> _object;
};

inline bool operator==(option<Key> key0, option<Key> key1)
{
    lateref<Key> nonNullKey0;
    if (key0.isNotNull(nonNullKey0))
    {
        return nonNullKey0->equal(key1);
    }

    return key1 == nullptr;
}
