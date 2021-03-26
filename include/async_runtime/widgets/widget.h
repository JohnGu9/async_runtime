#pragma once

#include "../elements/element.h"
#include "../elements/key.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(option<Key> key);
    virtual bool canUpdate(ref<Widget> other);
    virtual ref<Element> createElement() = 0;

protected:
    const option<Key> _key;

public:
    const option<Key> &key = _key;
};

