#pragma once

#include "../elements/element.h"
#include "../elements/key.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(Object::Ref<Key> key);
    virtual bool canUpdate(Object::Ref<Widget> other);
    virtual Object::Ref<Element> createElement() = 0;

protected:
    const Object::Ref<Key> _key;

public:
    const Object::Ref<Key> &key = _key;
};

