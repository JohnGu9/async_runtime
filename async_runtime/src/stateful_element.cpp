#include "framework/widgets/widget.h"
#include "framework/elements/stateful_element.h"

StatefulElement::StatefulElement(Object::Ref<StatefulWidget> widget) : _statefulWidget(widget), Element(widget)
{
    _state = _statefulWidget->createState();
    _state->widget = _statefulWidget;
}

void StatefulElement::build()
{
    Object::Ref<Widget> widget = this->_state->build(this->cast<BuildContext>());
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
        _childElement->parent = this->cast<Element>();
        _childElement->attach();
        _childElement->build();
    }
}

void StatefulElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<Widget> oldWidget = this->widget;
    this->widget = newWidget;
    this->_state->didWidgetUpdated(oldWidget->cast<StatefulWidget>());
}

void StatefulElement::attach()
{
    Element::attach();
    this->_state->initState();
    this->_state->context = this->cast<BuildContext>();
    this->_state->mounted = true;
    this->_state->didDependenceChanged();
}

void StatefulElement::detach()
{
    this->_childElement->detach();
    this->_state->dispose();
    this->_state->mounted = false;
    this->_state = nullptr;
    this->_childElement = nullptr;
    this->_statefulWidget = nullptr;
    Element::detach();
}