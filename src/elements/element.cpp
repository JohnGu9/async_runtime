#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

/// Element
Element::Element(Object::Ref<Widget> widget) : BuildContext(widget) {}

void Element::attach()
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances;
    if (Object::Ref<Key> key = this->widget->getKey())
        key->setElement(Object::self(this));
}

void Element::detach()
{
    this->_inheritances.clear();
    if (Object::Ref<Key> key = this->widget->getKey())
        key->setElement(nullptr);
    this->widget = nullptr;
}

void Element::update(Object::Ref<Widget> newWidget)
{
    this->widget = newWidget;
    this->build();
}

void Element::notify(Object::Ref<Widget> newWidget)
{
    this->widget = newWidget;
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances; // update inheritances
}

void Element::visitAncestor(Fn<bool(Object::Ref<Element>)> fn)
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    if (fn(parent) == false)
        parent->visitAncestor(fn);
}