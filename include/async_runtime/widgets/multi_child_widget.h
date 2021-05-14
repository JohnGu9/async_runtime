#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
    friend class MultiChildElement;

public:
    static ref<MultiChildWidget> fromChildren(ref<List<ref<Widget>>> children, option<Key> key = nullptr);
    MultiChildWidget(ref<List<ref<Widget>>> children, option<Key> key = nullptr)
        : Widget(key), _children(children) {}

    virtual ref<MultiChildWidget> setChildren(ref<List<ref<Widget>>>);

protected:
    ref<List<ref<Widget>>> _children;
    ref<Element> createElement() override;
};