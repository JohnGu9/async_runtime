#include "async_runtime/elements/lead_element.h"
#include "async_runtime/widgets/leaf_widget.h"

/// Leaf Element
LeafElement::LeafElement(Object::Ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::notify(Object::Ref<Widget> newWidget) {}

void LeafElement::update(Object::Ref<Widget> newWidget) {}

void LeafElement::visitDescendant(Function<bool(Object::Ref<Element>)>) {}