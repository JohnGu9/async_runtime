#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

/// Leaf Element
LeafElement::LeafElement(Object::Ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::notify(Object::Ref<Widget> newWidget) {}

void LeafElement::update(Object::Ref<Widget> newWidget) {}

void LeafElement::visitDescendant(Fn<bool(Object::Ref<Element>)>) {}