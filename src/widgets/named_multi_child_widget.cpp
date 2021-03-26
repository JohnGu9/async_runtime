#include "async_runtime/elements/named_multi_child_element.h"
#include "async_runtime/widgets/named_multi_child_widget.h"

ref<NamedMultiChildWidget> NamedMultiChildWidget::fromChildren(
    Map<String, lateref<Widget>> children, option<Key> key)
{
    return Object::create<NamedMultiChildWidget>(children, key);
}

ref<Element> NamedMultiChildWidget::createElement()
{
    return Object::create<NamedMultiChildElement>(Object::cast<>(this));
}