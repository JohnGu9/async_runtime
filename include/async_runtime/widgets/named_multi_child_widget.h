#pragma once

#include "widget.h"

class NamedMultiChildWidget : public Widget
{
    friend class NamedMultiChildElement;

public:
    static ref<NamedMultiChildWidget> fromChildren(Map<String, lateref<Widget>> children, option<Key> key = nullptr);
    NamedMultiChildWidget(Map<String, lateref<Widget>> children, option<Key> key = nullptr)
        : _children(children), Widget(key) { assert(_children && "children can't be null. "); }

protected:
    Map<String, lateref<Widget>> _children;
    ref<Element> createElement() override;
};