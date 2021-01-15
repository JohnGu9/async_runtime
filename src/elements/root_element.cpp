#include "framework/fundamental/logger.h"
#include "framework/elements/element.h"
#include "framework/widgets/widget.h"
#include "framework/widgets/key.h"

class _MockWidget : public Widget
{
public:
    static Object::Ref<Widget> factory();
    _MockWidget() : Widget(nullptr) {}
    Object::Ref<Element> createElement() override { return nullptr; }
};

Object::Ref<Widget> _MockWidget::factory()
{
    static Object::Ref<Widget> singleton = Object::create<_MockWidget>();
    return singleton;
}

/// Root Element
RootElement::RootElement(Object::Ref<Widget> widget) : SingleChildElement(nullptr)
{
    assert(widget != nullptr);
    this->_stdoutKey = Object::create<GlobalKey>();
    this->_child = Object::create<Scheduler>(
        Object::create<StdoutLogger>(
            widget, _stdoutKey));
}

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(Object::Ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    this->widget = _MockWidget::factory();
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

Logger::Handler RootElement::getStdoutHandler()
{
    Object::Ref<StatefulWidgetState> currentState = this->_stdoutKey->getCurrentState();
    if (currentState == nullptr)
        return nullptr;
    if (Object::Ref<_StdoutLoggerState> state = currentState->cast<_StdoutLoggerState>())
        return state->_handler;
    return nullptr;
}