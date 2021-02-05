#pragma once

#include "../elements/element.h"
#include "../elements/key.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(Object::Ref<Key> key);
    virtual Object::Ref<Key> getKey();
    virtual bool canUpdate(Object::Ref<Widget> other);
    virtual Object::Ref<Element> createElement() = 0;

protected:
    const Object::Ref<Key> _key;
};

inline Object::Ref<Key> Widget::getKey()
{
    return this->_key;
}
