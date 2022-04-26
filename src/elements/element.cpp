#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/key.h"
#include "async_runtime/widgets/widget.h"

Element::Element(ref<Widget> widget) : BuildContext(widget) {}

void Element::attach()
{
    ref<Element> parent = this->parent.assertNotNull();
    this->_inheritances = parent->_inheritances;
    lateref<Key> key;
    if (this->widget->key.isNotNull(key))
        key->setElement(self());
}

void Element::detach()
{
    lateref<Key> key;
    if (this->widget->key.isNotNull(key))
        key->dispose();
    static finalref<Map<Object::RuntimeType, lateref<InheritedWidget>>> _empty = Object::create<Map<Object::RuntimeType, lateref<InheritedWidget>>>();
    this->_inheritances = _empty; // release map reference
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
