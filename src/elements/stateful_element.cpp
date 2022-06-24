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

std::ostream &operator<<(std::ostream &os, const StatefulElement::LifeCycle::Value &value)
{
    switch (value)
    {
    case StatefulElement::LifeCycle::uninitialized:
        return os << "StatefulElement::LifeCycle::uninitialized";
    case StatefulElement::LifeCycle::mounted:
        return os << "StatefulElement::LifeCycle::mounted";
    case StatefulElement::LifeCycle::building:
        return os << "StatefulElement::LifeCycle::building";
    case StatefulElement::LifeCycle::unmount:
        return os << "StatefulElement::LifeCycle::unmount";
    default:
        throw NotImplementedError();
    }
}

const ref<List<StatefulElement::LifeCycle::Value>>
    StatefulElement::LifeCycle::values = {
        StatefulElement::LifeCycle::uninitialized,
        StatefulElement::LifeCycle::mounted,
        StatefulElement::LifeCycle::building,
        StatefulElement::LifeCycle::unmount};

ref<String> StatefulElement::LifeCycle::toString(StatefulElement::LifeCycle::Value value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

StatefulElement::StatefulElement(ref<StatefulWidget> widget)
    : SingleChildElement(widget),
      _statefulWidget(widget),
      _state(_statefulWidget->createState()),
      _setStateCallbacks(List<Function<void()>>::create()),
      _lifeCycle(StatefulElement::LifeCycle::uninitialized)
{
}

void StatefulElement::attach()
{
    this->_lifeCycle = StatefulElement::LifeCycle::building;
    Element::attach();
    this->_state->_element = self();
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
    this->detachElement();
    this->_state->dispose();

    // release ref avoid ref each other
    static finalref<StatefulElement> _invalidElement = Object::create<StatefulElement>(Object::create<StatefulElement::InvalidWidget>());
    this->_state->_context = _invalidElement;
    this->_state->_element = _invalidElement;
    this->_setStateCallbacks->clear();
    Element::detach();
}

void StatefulElement::build()
{
    this->_lifeCycle = StatefulElement::LifeCycle::building;
    for (const auto &fn : this->_setStateCallbacks)
        fn();
    this->_setStateCallbacks->clear();
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
    ref<StatefulWidget> oldWidget = this->_statefulWidget;
    this->_statefulWidget = newWidget->covariant<StatefulWidget>();
    this->_state->callDidWidgetUpdated(oldWidget);
    Element::update(newWidget);
}

void StatefulElement::notify(ref<Widget> newWidget)
{
    this->_lifeCycle = StatefulElement::LifeCycle::building;
    {
        Element::notify(newWidget);
        ref<StatefulWidget> oldWidget = this->_statefulWidget;
        this->_statefulWidget = newWidget->covariant<StatefulWidget>();
        this->_state->callDidWidgetUpdated(oldWidget);
        this->_state->didDependenceChanged();
    }
    {
        for (const auto &fn : this->_setStateCallbacks)
            fn();
        this->_setStateCallbacks->clear();
        ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
        ref<Widget> oldWidget = this->_childElement->getWidget();
        if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
            this->_childElement->notify(widget);
        else
            this->reattachElement(widget->createElement());
    }
    this->_lifeCycle = StatefulElement::LifeCycle::mounted;
}

void StatefulElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void StatefulElement::scheduleRebuild(Function<void()> fn, ref<EventLoop> loop)
{
    auto self = self();
    this->_setStateCallbacks->emplaceBack(fn);    // mark state dirty
    loop->callSoon([this, self] {                 //
        if (!this->_setStateCallbacks->isEmpty()) // if state is dirty, rebuild the tree
            this->build();
    });
}
