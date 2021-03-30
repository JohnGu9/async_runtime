#include "async_runtime/elements/lead_element.h"
#include "async_runtime/widgets/leaf_widget.h"

ref<LeafWidget> LeafWidget::factory()
{
    static ref<LeafWidget> singleton = Object::create<LeafWidget>();
    return singleton;
}

LeafWidget::LeafWidget(option<Key> key) : Widget(key){};

ref<Element> LeafWidget::createElement()
{
    return Object::create<LeafElement>(self());
}