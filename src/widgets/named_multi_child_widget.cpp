#include "async_runtime/widgets/named_multi_child_widget.h"
#include "async_runtime/elements/element.h"

Object::Ref<NamedMultiChildWidget> NamedMultiChildWidget::fromChildren(
    Map<String, Object::Ref<Widget>> children, Object::Ref<Key> key)
{
    return Object::create<NamedMultiChildWidget>(children, key);
}

Object::Ref<Element> NamedMultiChildWidget::createElement()
{
    return Object::create<NamedMultiChildElement>(Object::cast<>(this));
}