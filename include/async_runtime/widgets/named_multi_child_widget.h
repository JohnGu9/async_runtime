#pragma once

#include "widget.h"

class NamedMultiChildWidget : public Widget
{
    friend class NamedMultiChildElement;

public:
    static ref<NamedMultiChildWidget> fromChildren(Map<String, ref<Widget>> children, ref<Key> key = nullptr);
    NamedMultiChildWidget(Map<String, ref<Widget>> children, ref<Key> key = nullptr)
        : _children(children), Widget(key) { assert(_children && "children can't be null. "); }

protected:
    Map<String, ref<Widget>> _children;
    ref<Element> createElement() override;
};