#include "async_runtime/widgets/named_multi_child_widget.h"
#include "async_runtime/elements/named_multi_child_element.h"

ref<NamedMultiChildWidget> NamedMultiChildWidget::fromChildren(
    ref<Map<ref<String>, lateref<Widget>>> children, option<Key> key)
{
    return Object::create<NamedMultiChildWidget>(children, key);
}

NamedMultiChildWidget::NamedMultiChildWidget(ref<Map<ref<String>, lateref<Widget>>> children, option<Key> key)
    : Widget(key), _children(children) {}

ref<Element> NamedMultiChildWidget::createElement()
{
    return Object::create<NamedMultiChildElement>(self());
}
