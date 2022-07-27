#pragma once
#include "../object/object.h"

class Element;

class Key : public virtual Object
{
public:
    virtual bool isEqual(option<Key> other) = 0;
    virtual void setElement(ref<Element> element) {}
    virtual void dispose() {}
};

template <typename T>
class ValueKey : public Key
{
public:
    ValueKey(const T &value) : _value(value) {}
    ValueKey(T &&value) : _value(std::move(value)) {}

    bool isEqual(option<Key> other) override
    {
        if (auto ptr = dynamic_cast<ValueKey<T> *>(other.get()))
        {
            return this->_value == ptr->_value;
        }
        return false;
    }

protected:
    T _value;
};
