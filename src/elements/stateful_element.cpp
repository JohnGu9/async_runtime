#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/stateful_widget.h"

class StatefulElement::InvalidWidget : public StatefulWidget
{
    class _State;

public:
    InvalidWidget() : StatefulWidget(nullptr) {}
    ref<State<StatefulWidget>> createState() final;
};

class StatefulElement::InvalidWidget::_State : public State<StatefulElement::InvalidWidget>
{
    ref<Widget> build(ref<BuildContext>) final { throw std::runtime_error("AsyncRuntime Internal Error"); }
};

ref<State<StatefulWidget>> StatefulElement::InvalidWidget::createState() { return Object::create<StatefulElement::InvalidWidget::_State>(); }

const List<StatefulElement::LifeCycle::Value>
    StatefulElement::LifeCycle::values = {
        StatefulElement::LifeCycle::uninitialized,
        StatefulElement::LifeCycle::mounted,
        StatefulElement::LifeCycle::building,
        StatefulElement::LifeCycle::unmount};

ref<String> StatefulElement::LifeCycle::toString(StatefulElement::LifeCycle::Value value)
{
    switch (value)
    {
    case StatefulElement::LifeCycle::uninitialized:
        return "StatefulElement::LifeCycle::uninitialized";
    case StatefulElement::LifeCycle::mounted:
        return "StatefulElement::LifeCycle::mounted";
    case StatefulElement::LifeCycle::building:
        return "StatefulElement::LifeCycle::building";
    case StatefulElement::LifeCycle::unmount:
        return "StatefulElement::LifeCycle::unmount";
    default:
        assert(false && "The enum doesn't exists. ");
        return "The enum doesn't exists";
    }
}

StatefulElement::StatefulElement(ref<StatefulWidget> widget)
    : SingleChildElement(widget),
      _statefulWidget(widget),
      _state(_statefulWidget->createState()),
      _lifeCycle(StatefulElement::LifeCycle::uninitialized)
{
}

void StatefulElement::attach()
{
    this->_lifeCycle = StatefulElement::LifeCycle::building;
    assert(this->_state->_mounted == false && "This [State] class mount twice is not allowed. User should not reuse [State] class or manually call [initState]");
    Element::attach();
    this->_state->_element = self();
    this->_state->_mounted = true;
    this->_state->initState();
    this->_state->_context = self(); // context only available after initState
    this->_state->didDependenceChanged();
    ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    this->attachElement(widget->createElement());
    this->_lifeCycle = StatefulElement::LifeCycle::mounted;
}

void StatefulElement::detach()
{
    this->_lifeCycle = StatefulElement::LifeCycle::unmount;
    assert(this->_state->_mounted && "This [State] class dispose more then twice is not allowed. User should not reuse [State] class or manually call [dispose]");
    this->detachElement();
    this->_state->dispose();
    this->_state->_mounted = false;

    // release ref avoid ref each other
    static finalref<StatefulElement> _invalidElement = Object::create<StatefulElement>(Object::create<StatefulElement::InvalidWidget>());
    this->_state->_context = _invalidElement;
    this->_state->_element = _invalidElement;
    Element::detach();
}

void StatefulElement::build()
{
    this->_lifeCycle = StatefulElement::LifeCycle::building;
    assert(this->_state->_mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    ref<Widget> oldWidget = this->_childElement->getWidget();
    if (Object::identical(widget, oldWidget))
    {
        this->_lifeCycle = StatefulElement::LifeCycle::mounted;
        return;
    }
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
    this->_lifeCycle = StatefulElement::LifeCycle::mounted;
}

void StatefulElement::update(ref<Widget> newWidget)
{
    assert(this->_state->_mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    ref<StatefulWidget> oldWidget = this->_statefulWidget;
    this->_statefulWidget = newWidget->covariant<StatefulWidget>();
    this->_state->didWidgetUpdated(oldWidget);
    Element::update(newWidget);
}

void StatefulElement::notify(ref<Widget> newWidget)
{
    {
        assert(this->_state->_mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
        Element::notify(newWidget);
        ref<StatefulWidget> oldWidget = this->_statefulWidget;
        this->_statefulWidget = newWidget->covariant<StatefulWidget>();
        this->_state->didWidgetUpdated(oldWidget);
        this->_state->didDependenceChanged();
    }

    this->_lifeCycle = StatefulElement::LifeCycle::building;
    ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    ref<Widget> oldWidget = this->_childElement->getWidget();
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
    this->_lifeCycle = StatefulElement::LifeCycle::mounted;
}

void StatefulElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}