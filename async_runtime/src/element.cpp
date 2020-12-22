#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

Element::Element(Object::Ref<Widget> widget_) : widget(widget_) {}

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

void Element::notify()
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr);
    this->_inherits = parent->_inherits;
}

LeafElement::LeafElement(Object::Ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::notify() {}

void LeafElement::update(Object::Ref<Widget> newWidget) {}

RootElement::RootElement(Object::Ref<Widget> widget) : Element(nullptr), _child(widget) {}

void RootElement::build()
{
    Object::Ref<Widget> widget = _child;
    assert(widget != nullptr);
    Object::Ref<Widget> lastWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, lastWidget))
        return;
    else if (widget->canUpdate(lastWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->build();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(_childElement != nullptr);
        this->_childElement->parent = Object::cast<RootElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false); }

void RootElement::notify() { assert(false); }

void RootElement::attach()
{
    // RootElement
}

void RootElement::detach()
{
    this->_childElement->detach();
    this->_childElement = nullptr;
    Element::detach();
}

InheritedElement::InheritedElement(Object::Ref<InheritedWidget> widget) : _inheritWidget(widget), Element(widget) {}

void InheritedElement::attach()
{
    Element::attach();
    this->_inherits[this->_inheritWidget->runtimeType()] = Object::cast<Inherit>(this->_inheritWidget.get());
}

void InheritedElement::detach()
{
    this->_childElement->detach();
    this->_childElement = nullptr;
    this->_inheritWidget = nullptr;
    Element::detach();
}

void InheritedElement::build()
{
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr);
    Object::Ref<Widget> lastWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, lastWidget))
        return;
    else if (widget->canUpdate(lastWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->build();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(_childElement != nullptr);
        this->_childElement->parent = Object::cast<InheritedElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

void InheritedElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<InheritedWidget> castedWidget = newWidget->cast<InheritedWidget>();
    assert(castedWidget != nullptr);
    Object::Ref<InheritedWidget> oldWidget = this->_inheritWidget;
    this->widget = newWidget;
    this->_inheritWidget = castedWidget;
    if (castedWidget->updateShouldNotify(oldWidget))
        this->notify();
}

void InheritedElement::notify()
{
    this->_inherits[this->_inheritWidget->runtimeType()] = Object::cast<Inherit>(this->_inheritWidget.get());
    this->_childElement->notify();
    this->_childElement->build();
}
