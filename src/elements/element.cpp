#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/widget.h"

/// Element
Element::Element(ref<Widget> widget) : BuildContext(widget) {}

void Element::attach()
{
    ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances;
    if (ref<Key> key = this->widget->key)
        key->setElement(Object::cast<>(this));
}

void Element::detach()
{
    this->_inheritances = nullptr; // release map reference
    if (ref<Key> key = this->widget->key)
        key->dispose();
    this->widget = nullptr;
}

void Element::update(ref<Widget> newWidget)
{
    this->widget = newWidget;
    this->build();
}

void Element::notify(ref<Widget> newWidget)
{
    this->widget = newWidget;
    ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances; // update inheritances
}

void Element::visitAncestor(Function<bool(ref<Element>)> fn)
{
    ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    if (fn(parent) == false)
        parent->visitAncestor(fn);
}
