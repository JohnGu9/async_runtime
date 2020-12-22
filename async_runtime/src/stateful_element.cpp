#include "framework/widgets/widget.h"
#include "framework/elements/stateful_element.h"

StatefulElement::StatefulElement(Object::Ref<StatefulWidget> widget) : _statefulWidget(widget), Element(widget)
{
    _state = _statefulWidget->createState();
}

void StatefulElement::build()
{
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
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
        this->_childElement->parent = Object::cast<StatefulElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

void StatefulElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<StatefulWidget> castedWidget = newWidget->cast<StatefulWidget>();
    assert(castedWidget != nullptr);
    Object::Ref<StatefulWidget> oldWidget = this->_statefulWidget;
    this->widget = newWidget;
    this->_statefulWidget = castedWidget;
    this->_state->didWidgetUpdated(oldWidget);
}

void StatefulElement::attach()
{
    Element::attach();
    this->_state->element = Object::cast<StatefulElement>(this);
    this->_state->initState();
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

void StatefulElement::notify()
{
    Element::notify();
    this->_state->didDependenceChanged();
    this->_childElement->notify();
    this->_childElement->build();
}
