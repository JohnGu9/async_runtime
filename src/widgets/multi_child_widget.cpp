#include "async_runtime/elements/multi_child_element.h"
#include "async_runtime/widgets/multi_child_widget.h"

ref<MultiChildWidget> MultiChildWidget::fromChildren(List<ref<Widget>> children, option<Key> key)
{
    return Object::create<MultiChildWidget>(children, key);
}

ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(self());
}

ref<MultiChildWidget> MultiChildWidget::setChildren(List<ref<Widget>> children)
{
    this->_children = std::forward<List<ref<Widget>>>(children);
    return self();
}