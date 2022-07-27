#pragma once
#include "../object/object.h"

class Element;
class Key;

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
