#include "async_runtime/elements/inherited_element.h"
#include "async_runtime/widgets/inherited_widget.h"

InheritedElement::InheritedElement(ref<InheritedWidget> widget) : SingleChildElement(widget), _inheritWidget(widget) {}

void InheritedElement::attach()
{
    Element::attach();
    this->_inheritances = this->_inheritances->copy();
    this->_inheritances[this->_inheritWidget->runtimeType()] = this->_inheritWidget;
    ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    this->attachElement(widget->createElement());
}

void InheritedElement::detach()
{
    this->detachElement();
    this->_inheritances->clear();
    Element::detach();
}

void InheritedElement::build()
{
    ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    ref<Widget> oldWidget = this->_childElement->getWidget();
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void InheritedElement::update(ref<Widget> newWidget)
{
    ref<InheritedWidget> oldWidget = this->_inheritWidget;
    this->_inheritWidget = newWidget->covariant<InheritedWidget>();
    if (this->_inheritWidget->updateShouldNotify(oldWidget))
        this->notify(newWidget);
    else
        Element::update(newWidget);
}

void InheritedElement::notify(ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_inheritances = this->_inheritances->copy();
    this->_inheritances[this->_inheritWidget->runtimeType()] = this->_inheritWidget;
    this->_inheritWidget = newWidget->covariant<InheritedWidget>();

    ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    ref<Widget> oldWidget = this->_childElement->getWidget();
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
}

void InheritedElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}
