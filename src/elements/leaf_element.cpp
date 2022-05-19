#include "async_runtime/elements/lead_element.h"
#include "async_runtime/widgets/leaf_widget.h"

LeafElement::LeafElement(ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::notify(ref<Widget> newWidget) {}

void LeafElement::update(ref<Widget> newWidget) {}

void LeafElement::visitDescendant(Function<bool(ref<Element>)>) {}
