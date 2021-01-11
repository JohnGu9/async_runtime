#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

class _MockWidget : public Widget
{
public:
    _MockWidget() : Widget(nullptr) {}
    Object::Ref<Element> createElement() { return nullptr; }
};

/// Root Element
RootElement::RootElement(Object::Ref<Widget> widget) : SingleChildElement(nullptr), _child(widget) { assert(widget != nullptr); }

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(Object::Ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    this->widget = Object::create<_MockWidget>();
    this->attachElement(this->_child->createElement());
}

void RootElement::build()
{
    assert(this->_childElement != nullptr);
    Object::Ref<Widget> widget = this->_child;
    Object::Ref<Widget> lastWidget = this->_childElement->widget;
    if (Object::identical(widget, lastWidget))
        return;
    else if (widget->canUpdate(lastWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void RootElement::detach()
{
    this->detachElement();
    Element::detach();
}

void RootElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    assert(this->_childElement != nullptr);
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void RootElement::visitAncestor(Fn<bool(Object::Ref<Element>)>) {}