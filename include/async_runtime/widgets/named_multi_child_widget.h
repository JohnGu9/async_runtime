#pragma once

#include "widget.h"

class NamedMultiChildWidget : public Widget
{
    friend class NamedMultiChildElement;

public:
    static ref<NamedMultiChildWidget> fromChildren(Map<ref<String>, lateref<Widget>> children, option<Key> key = nullptr);
    NamedMultiChildWidget(Map<ref<String>, lateref<Widget>> children, option<Key> key = nullptr)
        : Widget(key), _children(children) { assert(_children && "children can't be null. "); }

protected:
    Map<ref<String>, lateref<Widget>> _children;
    ref<Element> createElement() override;
};