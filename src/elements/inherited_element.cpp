#include "async_runtime/elements/inherited_element.h"
#include "async_runtime/widgets/inherited_widget.h"

/// Inherited Element
InheritedElement::InheritedElement(ref<InheritedWidget> widget) : _inheritWidget(widget), SingleChildElement(widget) {}

void InheritedElement::attach()
{
    Element::attach();
    this->_inheritances = this->_inheritances.copy();
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
    ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    this->attachElement(widget->createElement());
}

void InheritedElement::detach()
{
    this->detachElement();
    this->_inheritWidget = nullptr;
    Element::detach();
}

void InheritedElement::build()
{
    assert(this->_childElement != nullptr);
    ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    ref<Widget> oldWidget = this->_childElement->widget;
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
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
    this->_inheritWidget = newWidget->covariant<InheritedWidget>();
    assert(this->_childElement != nullptr);

    ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    ref<Widget> oldWidget = this->_childElement->widget;
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
