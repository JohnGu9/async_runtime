#include "async_runtime/widgets/multi_child_widget.h"

MultiChildWidget::MultiChildWidget(const Object::List<Object::Ref<Widget>> &children, Object::Ref<Key> key) : _children(children), Widget(key) {}

Object::Ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(Object::cast<>(this));
}
