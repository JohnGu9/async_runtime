#pragma once

#include "widget.h"

class MultiChildWidget : public Widget
{
    friend MultiChildElement;

public:
    static Object::Ref<MultiChildWidget> fromChildren(List<Object::Ref<Widget>> &&children, Object::Ref<Key> key = nullptr);
    MultiChildWidget(const List<Object::Ref<Widget>> &children, Object::Ref<Key> key = nullptr)
        : _children(children), Widget(key) {}
    MultiChildWidget(List<Object::Ref<Widget>> &&children, Object::Ref<Key> key = nullptr)
        : _children(std::forward<List<Object::Ref<Widget>>>(children)), Widget(key) {}
    MultiChildWidget(Object::Ref<Key> key = nullptr) : _children(), Widget(key) {}

    Object::Ref<Element> createElement() override;
    virtual Object::Ref<MultiChildWidget> setChildren(List<Object::Ref<Widget>> &&);

protected:
    List<Object::Ref<Widget>> _children;
};