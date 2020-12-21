#pragma once

#include <list>
#include "../object.h"

class Key : public virtual Object
{
public:
    virtual bool equal(Object::Ref<Key> object) = 0;
};

template <typename T>
class ValueKey : public Key
{
public:
    ValueKey(T &value) : _value(value) {}
    virtual bool equal(Object::Ref<Key> object) override
    {
        Key *pointer = object.get();
        if (ValueKey<T> *castedPointer = dynamic_cast<ValueKey<T> *>(pointer))
            return castedPointer->_value == _value;
        return false;
    }

protected:
    T _value;
};

class GlobalKey : public Key
{
};

class GlobalObjectKey : public Key
{
public:
    GlobalObjectKey(Object::Ref<Object> object) : _object(object){};
    virtual bool equal(Object::Ref<Key> object);

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

inline bool GlobalObjectKey::equal(Object::Ref<Key> object)
{
    if (GlobalObjectKey *castedPointer = dynamic_cast<GlobalObjectKey *>(object.get()))
        return Object::identical(_object, castedPointer->_object);
    return false;
}