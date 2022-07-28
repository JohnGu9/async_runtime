#pragma once
#include "../object/object.h"

class Element;
class Key;

class Widget : public virtual Object
{
protected:
    const option<Key> _key;

public:
    Widget(option<Key> key);
    Widget() = delete;

    virtual bool canUpdate(ref<Widget> other);
    virtual ref<Element> createElement() = 0;
    const option<Key> &key = _key;
};
