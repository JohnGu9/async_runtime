#pragma once

#include <list>
#include "element.h"

class Widget;

class Key : public Object
{
public:
    virtual bool equal(ref<Key> other) = 0;
    virtual void setElement(ref<Element> element);
    virtual void dispose();
    virtual ref<const Widget> getCurrentWidget();

protected:
    virtual ref<Element> getElement();

private:
    weakref<Element> _element;
};

template <typename T>
class ValueKey : public Key
{
public:
    ValueKey(T &value) : _value(value) {}
    virtual bool equal(ref<Key> other) override
    {
        if (auto castedPointer = other->cast<ValueKey<T>>())
            return castedPointer->_value == this->_value;
        return false;
    }

protected:
    T _value;
};

class GlobalKey : public Key
{
public:
    inline virtual bool equal(ref<Key> other)
    {
        return Object::identical(this->shared_from_this(), other);
    }

    virtual ref<BuildContext> getCurrentContext();
    virtual ref<State<StatefulWidget>> getCurrentState();
};

class GlobalObjectKey : public GlobalKey
{
public:
    GlobalObjectKey(ref<Object> object) : _object(object){};
    virtual bool equal(ref<Key> other)
    {
        if (auto castedPointer = other->cast<GlobalObjectKey>())
            return Object::identical(this->_object, castedPointer->_object);
        return false;
    }

protected:
    GlobalObjectKey() = delete;
    ref<Object> _object;
};

inline bool operator==(ref<Key> key0, ref<Key> key1)
{
    if (key0 == nullptr)
        return key1 == nullptr;
    return key0->equal(key1);
}
