#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

Element::Element(Object::Ref<Widget> widget_) : BuildContext(), widget(widget_) {}

void Element::attach()
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr);
    this->_inherits = parent->_inherits;
}

void Element::detach()
{
    this->_inherits.clear();
}

LeafElement::LeafElement(Object::Ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::update(Object::Ref<Widget> newWidget) {}

RootElement::RootElement(Object::Ref<Widget> widget) : Element(nullptr), childWidget(widget) {}

void RootElement::build()
{
    Object::Ref<Widget> widget = childWidget;
    assert(widget != nullptr);
    Object::Ref<Widget> lastWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, lastWidget))
        return;
    else if (widget->equal(lastWidget))
    {
        _childElement->update(widget);
        _childElement->build();
    }
    else
    {
        if (_childElement != nullptr)
            _childElement->detach();
        _childElement = widget->createElement();
        _childElement->parent = Object::cast<RootElement>(this);
        _childElement->attach();
        _childElement->build();
    }
}

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false); }

void RootElement::attach()
{
    // RootElement
}

void RootElement::detach()
{
    this->_childElement->detach();
    Element::detach();
}