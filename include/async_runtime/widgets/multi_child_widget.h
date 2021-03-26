#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
    friend class MultiChildElement;

public:
    static ref<MultiChildWidget> fromChildren(List<ref<Widget>> children, option<Key> key = nullptr);
    MultiChildWidget(List<ref<Widget>> children, option<Key> key = nullptr)
        : _children(children), Widget(key) { assert(_children && "children can't be null. "); }
    MultiChildWidget(option<Key> key = nullptr) : _children({}), Widget(key) { assert(_children && "children can't be null. "); }

    virtual ref<MultiChildWidget> setChildren(List<ref<Widget>>);

protected:
    List<ref<Widget>> _children;
    ref<Element> createElement() override;
};