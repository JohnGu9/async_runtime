#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

/// Stateful Element
StatefulElement::StatefulElement(Object::Ref<StatefulWidget> widget)
    : _statefulWidget(widget), SingleChildElement(widget)
{
    _state = _statefulWidget->createState();
}

void StatefulElement::attach()
{
    assert(this->_state->mounted == false && "This [State] class mount twice is not allowed. User should not reuse [State] class or manually call [initState]");

    Element::attach();
    this->_state->element = Object::cast<StatefulElement>(this);
    this->_state->initState();
    this->_state->mounted = true;
    this->_state->didDependenceChanged();

    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    this->attachElement(widget->createElement());
}

void StatefulElement::detach()
{
    assert(this->_state->mounted && "This [State] class dispose more than twice is not allowed. User should not reuse [State] class or manually call [dispose]");
    this->detachElement();
    this->_state->dispose();
    this->_state->mounted = false;
    this->_state = nullptr;
    this->_statefulWidget = nullptr;
    Element::detach();
}

void StatefulElement::build()
{
    assert(this->_childElement != nullptr);
    assert(this->_state->mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void StatefulElement::update(Object::Ref<Widget> newWidget)
{
    assert(this->_state->mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    Object::Ref<StatefulWidget> oldWidget = this->_statefulWidget;
    this->_statefulWidget = newWidget->cast<StatefulWidget>();
    assert(this->_statefulWidget != nullptr);
    this->_state->didWidgetUpdated(oldWidget);
    Element::update(newWidget);
}

void StatefulElement::notify(Object::Ref<Widget> newWidget)
{
    assert(this->_childElement != nullptr);
    assert(this->_state->mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");

    Element::notify(newWidget);
    {
        Object::Ref<StatefulWidget> oldWidget = this->_statefulWidget;
        this->_statefulWidget = newWidget->cast<StatefulWidget>();
        assert(this->_statefulWidget != nullptr);
        this->_state->didWidgetUpdated(oldWidget);
    }
    this->_state->didDependenceChanged();
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
}

void StatefulElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}