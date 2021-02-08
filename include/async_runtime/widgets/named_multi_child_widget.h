#pragma once

#include "widget.h"

class NamedMultiChildWidget : public Widget
{
    friend class NamedMultiChildElement;

public:
    static Object::Ref<NamedMultiChildWidget> fromChildren(Map<String, Object::Ref<Widget>> children, Object::Ref<Key> key = nullptr);
    NamedMultiChildWidget(Map<String, Object::Ref<Widget>> children, Object::Ref<Key> key = nullptr)
        : _children(children), Widget(key) { assert(_children && "children can't be null. "); }

protected:
    Map<String, Object::Ref<Widget>> _children;
    Object::Ref<Element> createElement() override;
};