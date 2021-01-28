#include "async_runtime/widgets/multi_child_widget.h"

Object::Ref<MultiChildWidget> MultiChildWidget::fromChildren(List<Object::Ref<Widget>> &&children, Object::Ref<Key> key)
{
    return Object::create<MultiChildWidget>(std::forward<List<Object::Ref<Widget>>>(children), key);
}

Object::Ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(Object::cast<>(this));
}

Object::Ref<MultiChildWidget> MultiChildWidget::setChildren(List<Object::Ref<Widget>> &&children)
{
    this->_children = std::forward<List<Object::Ref<Widget>>>(children);
    return Object::cast<>(this);
}