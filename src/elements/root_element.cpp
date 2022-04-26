#include "async_runtime/elements/root_element.h"
#include "async_runtime/fundamental/thread.h"

#include "async_runtime/widgets/key.h"
#include "async_runtime/widgets/logger_widget.h"
#include "async_runtime/widgets/stateful_widget.h"

class RootElement::RootWidget : public Widget
{
public:
    RootWidget() : Widget(nullptr) {}
    ref<Element> createElement() override
    {
        return Object::create<RootElement>(self(), [] {});
    }
};

class RootElement::RootFundamental : public StatelessWidget
{

    ref<Widget> child;
    RootElement *rootElement;

public:
    class RootInteritedWidget : public InheritedWidget
    {
    public:
        RootElement *rootElement;
        RootInteritedWidget(ref<Widget> child, RootElement *rootElement) : InheritedWidget(child), rootElement(rootElement) {}
        bool updateShouldNotify(ref<InheritedWidget> oldWidget) override
        {
            return oldWidget->covariant<RootInteritedWidget>()->rootElement != this->rootElement;
        }
    };

    RootFundamental(ref<Widget> child, RootElement *rootElement, option<Key> key = nullptr)
        : StatelessWidget(key), child(child), rootElement(rootElement) {}

    ref<Widget> build(ref<BuildContext> context) override
    {
        return Object::create<RootInteritedWidget>(child, rootElement);
    }
};

ref<RootElement> RootElement::of(ref<BuildContext> context)
{
    return Object::cast<>(context
                              ->dependOnInheritedWidgetOfExactType<RootElement::RootFundamental::RootInteritedWidget>()
                              .assertNotNull()
                              ->rootElement);
}

/**
 * @brief Construct a new Root Element:: Root Element object
 *
 * @param widget
 */
RootElement::RootElement(ref<Widget> widget, Function<void()> exit)
    : SingleChildElement(Object::create<RootWidget>()), exit(exit),
      _child(Object::create<RootFundamental>(widget, this)) {}

void RootElement::update(ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    static finalref<Map<Object::RuntimeType, lateref<InheritedWidget>>> empty = Object::create<Map<Object::RuntimeType, lateref<InheritedWidget>>>();
    this->_inheritances = empty;
    this->attachElement(this->_child->createElement());
}

void RootElement::build()
{
    ref<Widget> widget = this->_child;
    ref<Widget> lastWidget = this->_childElement->getWidget();
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

void RootElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void RootElement::visitAncestor(Function<bool(ref<Element>)>) {}
