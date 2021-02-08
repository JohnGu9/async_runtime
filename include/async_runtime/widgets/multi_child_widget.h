#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
    friend MultiChildElement;

public:
    static Object::Ref<MultiChildWidget> fromChildren(List<Object::Ref<Widget>> children, Object::Ref<Key> key = nullptr);
    MultiChildWidget(List<Object::Ref<Widget>> children, Object::Ref<Key> key = nullptr)
        : _children(children), Widget(key) { assert(_children && "children can't be null. "); }
    MultiChildWidget(Object::Ref<Key> key = nullptr) : _children({}), Widget(key) { assert(_children && "children can't be null. "); }

    virtual Object::Ref<MultiChildWidget> setChildren(List<Object::Ref<Widget>>);

protected:
    List<Object::Ref<Widget>> _children;
    Object::Ref<Element> createElement() override;
};