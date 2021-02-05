#pragma once

#include <list>
#include "element.h"

class Widget;

class Key : public Object
{
public:
    virtual bool equal(Object::Ref<Key> other) = 0;
    virtual void setElement(Object::Ref<Element> element);
    virtual void dispose();
    virtual Object::Ref<const Widget> getCurrentWidget();

protected:
    virtual Object::Ref<Element> getElement();

private:
    Object::WeakRef<Element> _element;
};

template <typename T>
class ValueKey : public Key
{
public:
    ValueKey(T &value) : _value(value) {}
    virtual bool equal(Object::Ref<Key> other) override
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
    inline virtual bool equal(Object::Ref<Key> other)
    {
        return Object::identical(this->shared_from_this(), other);
    }

    virtual Object::Ref<BuildContext> getCurrentContext();
    virtual Object::Ref<State<StatefulWidget>> getCurrentState();
};

class GlobalObjectKey : public GlobalKey
{
public:
    GlobalObjectKey(Object::Ref<Object> object) : _object(object){};
    virtual bool equal(Object::Ref<Key> other)
    {
        if (auto castedPointer = other->cast<GlobalObjectKey>())
            return Object::identical(this->_object, castedPointer->_object);
        return false;
    }

protected:
    GlobalObjectKey() = delete;
    Object::Ref<Object> _object;
};

inline bool operator==(Object::Ref<Key> key0, Object::Ref<Key> key1)
{
    if (key0 == nullptr)
        return key1 == nullptr;
    return key0->equal(key1);
}
