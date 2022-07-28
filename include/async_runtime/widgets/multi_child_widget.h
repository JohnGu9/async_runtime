#pragma once
#include "widget.h"

class MultiChildWidget : public Widget
{
    friend class MultiChildElement;

public:
    static ref<MultiChildWidget> fromChildren(ref<List<ref<Widget>>> children, option<Key> key = nullptr);
    MultiChildWidget(ref<List<ref<Widget>>> children, option<Key> key = nullptr);

protected:
    finalref<List<ref<Widget>>> _children;
    ref<Element> createElement() override;
};
