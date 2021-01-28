#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/inherited_widget.h"

/// Inherited Element
InheritedElement::InheritedElement(Object::Ref<InheritedWidget> widget) : _inheritWidget(widget), SingleChildElement(widget) {}

void InheritedElement::attach()
{
    Element::attach();
    this->_inheritances = Map<Object::RuntimeType, Object::Ref<Inheritance>>(this->_inheritances);
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
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
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void InheritedElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<InheritedWidget> castedWidget = newWidget->cast<InheritedWidget>();
    assert(castedWidget != nullptr);
    Object::Ref<InheritedWidget> oldWidget = this->_inheritWidget;
    this->_inheritWidget = castedWidget;
    if (castedWidget->updateShouldNotify(oldWidget))
        this->notify(newWidget);
    else
        Element::update(newWidget);
}

void InheritedElement::notify(Object::Ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
    this->_inheritWidget = newWidget->cast<InheritedWidget>();
    assert(this->_inheritWidget != nullptr);
    assert(this->_childElement != nullptr);

    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
}

void InheritedElement::visitDescendant(Function<bool(Object::Ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}