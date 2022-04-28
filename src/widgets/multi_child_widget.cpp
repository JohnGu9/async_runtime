#include "async_runtime/widgets/multi_child_widget.h"
#include "async_runtime/elements/multi_child_element.h"

ref<MultiChildWidget> MultiChildWidget::fromChildren(ref<List<ref<Widget>>> children, option<Key> key)
{
    return Object::create<MultiChildWidget>(children, key);
}

ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(self());
}
