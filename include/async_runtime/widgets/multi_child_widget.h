#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
    friend MultiChildElement;

public:
    MultiChildWidget(const Object::List<Object::Ref<Widget>> &children, Object::Ref<Key> key = nullptr);
    MultiChildWidget(Object::Ref<Key> key = nullptr);
    Object::Ref<Element> createElement() override;
    virtual Object::Ref<MultiChildWidget> setChildren(Object::List<Object::Ref<Widget>> &&);

protected:
    Object::List<Object::Ref<Widget>> _children;
};