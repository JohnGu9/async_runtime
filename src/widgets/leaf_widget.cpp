#include "async_runtime/elements/lead_element.h"
#include "async_runtime/widgets/leaf_widget.h"

Object::Ref<LeafWidget> LeafWidget::factory()
{
    static Object::Ref<LeafWidget> singleton = Object::create<LeafWidget>();
    return singleton;
}

LeafWidget::LeafWidget(Object::Ref<Key> key) : Widget(key){};

Object::Ref<Element> LeafWidget::createElement()
{
    return Object::create<LeafElement>(Object::cast<>(this));
}