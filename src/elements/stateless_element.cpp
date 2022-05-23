#include "async_runtime/elements/stateless_element.h"
#include "async_runtime/widgets/stateless_widget.h"

StatelessElement::StatelessElement(ref<StatelessWidget> widget)
    : SingleChildElement(widget), _statelessWidget(widget) {}

void StatelessElement::attach()
{
    Element::attach();
    ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    this->attachElement(widget->createElement());
}

void StatelessElement::detach()
{
    this->detachElement();
    Element::detach();
}

void StatelessElement::build()
{
    ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    ref<Widget> oldWidget = this->_childElement->getWidget();
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void StatelessElement::update(ref<Widget> newWidget)
{
    this->_statelessWidget = newWidget->covariant<StatelessWidget>();
    Element::update(newWidget);
}

void StatelessElement::notify(ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_statelessWidget = newWidget->covariant<StatelessWidget>();
    ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    ref<Widget> oldWidget = this->_childElement->getWidget();
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
}

void StatelessElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}
