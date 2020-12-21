#include "framework/widgets/widget.h"
#include "framework/elements/stateless_element.h"

StatelessElement::StatelessElement(Object::Ref<StatelessWidget> widget) : _statelessWidget(widget), Element(widget) {}

void StatelessElement::attach()
{
    Element::attach();
}

void StatelessElement::detach()
{
    _statelessWidget = nullptr;
    Element::detach();
}

void StatelessElement::build()
{
    Object::Ref<Widget> widget = this->_statelessWidget->build(this->cast<BuildContext>());
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

void StatelessElement::update(Object::Ref<Widget> newWidget)
{
    this->widget = newWidget;
}