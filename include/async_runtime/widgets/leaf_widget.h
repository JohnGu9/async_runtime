#pragma once

#include "widget.h"

class LeafWidget : public Widget
{
public:
    static Object::Ref<LeafWidget> factory();
    LeafWidget(Object::Ref<Key> key = nullptr);
    Object::Ref<Element> createElement() override;
};