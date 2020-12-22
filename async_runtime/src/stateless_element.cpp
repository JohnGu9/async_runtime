#include "framework/widgets/widget.h"
#include "framework/elements/stateless_element.h"

StatelessElement::StatelessElement(Object::Ref<StatelessWidget> widget) : _statelessWidget(widget), Element(widget) {}

void StatelessElement::detach()
{
    this->_childElement->detach();
    this->_childElement = nullptr;
    this->_statelessWidget = nullptr;
    Element::detach();
}

void StatelessElement::build()
{
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
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
        this->_childElement->parent = Object::cast<StatelessElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

void StatelessElement::update(Object::Ref<Widget> newWidget)
{
    this->_statelessWidget = newWidget->cast<StatelessWidget>();
    assert(this->_statelessWidget);
    this->widget = newWidget;
}

void StatelessElement::notify()
{
    Element::notify();
    this->_childElement->notify();
    this->_childElement->build();
}
