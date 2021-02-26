#pragma once

#include "widget.h"

class LeafWidget : public Widget
{
public:
    static ref<LeafWidget> factory();
    LeafWidget(ref<Key> key = nullptr);
    ref<Element> createElement() override;
};