#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/stateful_widget.h"

const List<StatefulElement::_LifeCycle::Value>
    StatefulElement::_LifeCycle::values = {
        StatefulElement::_LifeCycle::uninitialized,
        StatefulElement::_LifeCycle::mounted,
        StatefulElement::_LifeCycle::building,
        StatefulElement::_LifeCycle::unmount};

String StatefulElement::_LifeCycle::toString(StatefulElement::_LifeCycle::Value value)
{
    switch (value)
    {
    case StatefulElement::_LifeCycle::uninitialized:
        return "StatefulElement::_LifeCycle::uninitialized";
    case StatefulElement::_LifeCycle::mounted:
        return "StatefulElement::_LifeCycle::mounted";
    case StatefulElement::_LifeCycle::building:
        return "StatefulElement::_LifeCycle::building";
    case StatefulElement::_LifeCycle::unmount:
        return "StatefulElement::_LifeCycle::unmount";
    default:
        assert(false && "The enum doesn't exists. ");
        break;
    }
}

/// Stateful Element
StatefulElement::StatefulElement(ref<StatefulWidget> widget)
    : _statefulWidget(widget), SingleChildElement(widget), _lifeCycle(StatefulElement::_LifeCycle::uninitialized)
{
    _state = _statefulWidget->createState();
}

void StatefulElement::attach()
{
    this->_lifeCycle = StatefulElement::_LifeCycle::building;
    assert(this->_state->_mounted == false && "This [State] class mount twice is not allowed. User should not reuse [State] class or manually call [initState]");
    Element::attach();
    this->_state->_element = Object::cast<>(this);
    this->_state->_mounted = true;
    this->_state->initState();
    this->_state->_context = this->_state->_element; // context only available after initState
    this->_state->didDependenceChanged();
    ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    this->attachElement(widget->createElement());
    this->_lifeCycle = StatefulElement::_LifeCycle::mounted;
}

void StatefulElement::detach()
{
    this->_lifeCycle = StatefulElement::_LifeCycle::unmount;
    assert(this->_state->_mounted && "This [State] class dispose more than twice is not allowed. User should not reuse [State] class or manually call [dispose]");
    this->detachElement();
    this->_state->dispose();
    this->_state->_mounted = false;
    this->_state->_context = nullptr;
    this->_state->_element = nullptr;
    this->_state = nullptr;
    this->_statefulWidget = nullptr;
    Element::detach();
}

void StatefulElement::build()
{
    this->_lifeCycle = StatefulElement::_LifeCycle::building;
    assert(this->_childElement != nullptr);
    assert(this->_state->_mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
    this->_lifeCycle = StatefulElement::_LifeCycle::mounted;
}

void StatefulElement::update(ref<Widget> newWidget)
{
    this->_lifeCycle = StatefulElement::_LifeCycle::building;
    assert(this->_state->_mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    ref<StatefulWidget> oldWidget = this->_statefulWidget;
    this->_statefulWidget = newWidget->covariant<StatefulWidget>();
    this->_state->didWidgetUpdated(oldWidget);
    Element::update(newWidget);
    this->_lifeCycle = StatefulElement::_LifeCycle::mounted;
}

void StatefulElement::notify(ref<Widget> newWidget)
{
    this->_lifeCycle = StatefulElement::_LifeCycle::building;
    assert(this->_childElement != nullptr);
    assert(this->_state->_mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    Element::notify(newWidget);
    {
        ref<StatefulWidget> oldWidget = this->_statefulWidget;
        this->_statefulWidget = newWidget->covariant<StatefulWidget>();
        this->_state->didWidgetUpdated(oldWidget);
    }
    this->_state->didDependenceChanged();
    ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
    this->_lifeCycle = StatefulElement::_LifeCycle::mounted;
}

void StatefulElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}