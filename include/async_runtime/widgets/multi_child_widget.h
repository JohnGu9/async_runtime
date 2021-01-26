#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
    friend MultiChildElement;

public:
    static Object::Ref<MultiChildWidget> fromChildren(Object::List<Object::Ref<Widget>> &&children, Object::Ref<Key> key = nullptr);
    MultiChildWidget(const Object::List<Object::Ref<Widget>> &children, Object::Ref<Key> key = nullptr)
        : _children(children), Widget(key) {}
    MultiChildWidget(Object::List<Object::Ref<Widget>> &&children, Object::Ref<Key> key = nullptr)
        : _children(std::forward<Object::List<Object::Ref<Widget>>>(children)), Widget(key) {}
    MultiChildWidget(Object::Ref<Key> key = nullptr) : _children(), Widget(key) {}

    Object::Ref<Element> createElement() override;
    virtual Object::Ref<MultiChildWidget> setChildren(Object::List<Object::Ref<Widget>> &&);

protected:
    Object::List<Object::Ref<Widget>> _children;
};