#include "async_runtime/widgets/multi_child_widget.h"

Object::Ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(Object::cast<>(this));
}

Object::Ref<MultiChildWidget> MultiChildWidget::setChildren(Object::List<Object::Ref<Widget>> &&children)
{
    this->_children = std::forward<Object::List<Object::Ref<Widget>>>(children);
    return Object::cast<>(this);
}