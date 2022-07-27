#pragma once
#include "widget.h"

class NamedMultiChildWidget : public Widget
{
    friend class NamedMultiChildElement;

public:
    static ref<NamedMultiChildWidget> fromChildren(ref<Map<ref<String>, lateref<Widget>>> children, option<Key> key = nullptr);
    NamedMultiChildWidget(ref<Map<ref<String>, lateref<Widget>>> children, option<Key> key = nullptr)
        : Widget(key), _children(children) {}

protected:
    finalref<Map<ref<String>, lateref<Widget>>> _children;
    ref<Element> createElement() override;
};
