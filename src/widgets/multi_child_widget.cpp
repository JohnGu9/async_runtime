#include "async_runtime/widgets/multi_child_widget.h"

MultiChildWidget::MultiChildWidget(const Object::List<Object::Ref<Widget>> &children, Object::Ref<Key> key) : _children(children), Widget(key) {}

MultiChildWidget::MultiChildWidget(Object::Ref<Key> key) : _children({}), Widget(key) {}

Object::Ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(Object::cast<>(this));
}

Object::Ref<MultiChildWidget> MultiChildWidget::setChildren(Object::List<Object::Ref<Widget>> &&children)
{
    this->_children = std::forward<Object::List<Object::Ref<Widget>>>(children);
    return Object::cast<>(this);
}