#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/widget.h"

/// Element
Element::Element(ref<Widget> widget) : BuildContext(widget) {}

void Element::attach()
{
    ref<Element> parent = this->parent.assertNotNull();
    this->_inheritances = parent->_inheritances;
    lateref<Key> key;
    if (this->widget->key.isNotNull(key))
        key->setElement(Object::cast<>(this));
}

void Element::detach()
{
    this->_inheritances = nullptr; // release map reference
    lateref<Key> key;
    if (this->widget->key.isNotNull(key))
        key->dispose();
}

void Element::update(ref<Widget> newWidget)
{
    this->widget = newWidget;
    this->build();
}

void Element::notify(ref<Widget> newWidget)
{
    this->widget = newWidget;
    ref<Element> parent = this->parent.assertNotNull();
    this->_inheritances = parent->_inheritances; // update inheritances
}

void Element::visitAncestor(Function<bool(ref<Element>)> fn)
{
    ref<Element> parent = this->parent.assertNotNull();
    if (fn(parent) == false)
        parent->visitAncestor(fn);
}
