#include "async_runtime/elements/stateless_element.h"
#include "async_runtime/widgets/stateless_widget.h"

/// Stateless Element
StatelessElement::StatelessElement(ref<StatelessWidget> widget) : _statelessWidget(widget), SingleChildElement(widget) {}

void StatelessElement::attach()
{
    Element::attach();
    ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    this->attachElement(widget->createElement());
}

void StatelessElement::detach()
{
    this->detachElement();
    this->_statelessWidget = nullptr;
    Element::detach();
}

void StatelessElement::build()
{
    assert(this->_childElement != nullptr);
    ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    ref<Widget> oldWidget = this->_childElement->widget;
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
    assert(this->_childElement != nullptr);
    ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    ref<Widget> oldWidget = this->_childElement->widget;
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
