#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

/// Stateless Element
StatelessElement::StatelessElement(Object::Ref<StatelessWidget> widget) : _statelessWidget(widget), SingleChildElement(widget) {}

void StatelessElement::attach()
{
    Element::attach();
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
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
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void StatelessElement::update(Object::Ref<Widget> newWidget)
{
    this->_statelessWidget = newWidget->cast<StatelessWidget>();
    assert(this->_statelessWidget);
    Element::update(newWidget);
}

void StatelessElement::notify(Object::Ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_statelessWidget = newWidget->cast<StatelessWidget>();

    assert(this->_childElement != nullptr);
    assert(this->_statelessWidget);

    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;

    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
}

void StatelessElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}
