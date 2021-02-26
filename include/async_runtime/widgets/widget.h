#pragma once

#include "../elements/element.h"
#include "../elements/key.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(ref<Key> key);
    virtual bool canUpdate(ref<Widget> other);
    virtual ref<Element> createElement() = 0;

protected:
    const ref<Key> _key;

public:
    const ref<Key> &key = _key;
};

