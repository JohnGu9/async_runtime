#include "async_runtime/elements/multi_child_element.h"
#include "async_runtime/widgets/multi_child_widget.h"

ref<MultiChildWidget> MultiChildWidget::fromChildren(List<ref<Widget>> children, ref<Key> key)
{
    return Object::create<MultiChildWidget>(std::forward<List<ref<Widget>>>(children), key);
}

ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(Object::cast<>(this));
}

ref<MultiChildWidget> MultiChildWidget::setChildren(List<ref<Widget>> children)
{
    this->_children = std::forward<List<ref<Widget>>>(children);
    return Object::cast<>(this);
}