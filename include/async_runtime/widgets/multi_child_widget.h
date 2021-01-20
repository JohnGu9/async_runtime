#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
public:
    MultiChildWidget(const Object::List<Object::Ref<Widget>> &children, Object::Ref<Key> key = nullptr);
    Object::Ref<Element> createElement() override;

    friend MultiChildElement;

protected:
    Object::List<Object::Ref<Widget>> _children;
};