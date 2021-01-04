#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

/// Element
Element::Element(Object::Ref<Widget> widget_) : widget(widget_) {}

void Element::attach()
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances;
}

void Element::detach()
{
    this->_inheritances.clear();
}

void Element::notify()
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances; // update inheritances
}

/// Leaf Element
LeafElement::LeafElement(Object::Ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::notify() {}

void LeafElement::update(Object::Ref<Widget> newWidget) {}

/// Root Element
RootElement::RootElement(Object::Ref<Widget> widget) : Element(nullptr), _child(widget) {}

void RootElement::build()
{
    Object::Ref<Widget> widget = _child;
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
        assert(this->_childElement != nullptr);
        this->_childElement->parent = Object::cast<RootElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify() { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach() {}

void RootElement::detach()
{
    this->_childElement->detach();
    this->_childElement = nullptr;
    Element::detach();
}

/// Inherited Element
InheritedElement::InheritedElement(Object::Ref<InheritedWidget> widget) : _inheritWidget(widget), Element(widget) {}

void InheritedElement::attach()
{
    Element::attach();
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
}

void InheritedElement::detach()
{
    this->_childElement->detach();
    this->_childElement = nullptr;
    this->_inheritWidget = nullptr;
    Element::detach();
}

void InheritedElement::build()
{
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    Object::Ref<Widget> oldWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->build();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<InheritedElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

void InheritedElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<InheritedWidget> castedWidget = newWidget->cast<InheritedWidget>();
    assert(castedWidget != nullptr);
    Object::Ref<InheritedWidget> oldWidget = this->_inheritWidget;
    this->widget = newWidget;
    this->_inheritWidget = castedWidget;
    if (castedWidget->updateShouldNotify(oldWidget))
        this->notify();
}

void InheritedElement::notify()
{
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());

    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    Object::Ref<Widget> oldWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, oldWidget))
    {
        this->_childElement->notify();
    }
    else if (widget->canUpdate(oldWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->notify();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<InheritedElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

/// Stateful Element
StatefulElement::StatefulElement(Object::Ref<StatefulWidget> widget)
    : _statefulWidget(widget), Element(widget)
{
    _state = _statefulWidget->createState();
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

void StatefulElement::build()
{
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->build();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
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

void StatefulElement::notify()
{
    Element::notify();
    this->_state->didDependenceChanged();
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, oldWidget))
    {
        this->_childElement->notify();
    }
    else if (widget->canUpdate(oldWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->notify();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<StatefulElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

/// Stateless Element
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
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->build();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
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
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = _childElement == nullptr ? nullptr : _childElement->widget;
    if (Object::identical(widget, oldWidget))
    {
        this->_childElement->notify();
    }
    else if (widget->canUpdate(oldWidget))
    {
        this->_childElement->update(widget);
        this->_childElement->notify();
    }
    else
    {
        if (this->_childElement != nullptr)
            this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<StatelessElement>(this);
        this->_childElement->attach();
        this->_childElement->build();
    }
}

NotificationListenerElement::NotificationListenerElement(Object::Ref<NotificationListener> widget) : _notificationListenerWidget(widget), StatelessElement(widget), Element(widget) {}

void NotificationListenerElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<NotificationListener> castedWidget = newWidget->cast<NotificationListener>();
    assert(castedWidget != nullptr);
    this->_notificationListenerWidget = castedWidget;
    StatelessElement::update(newWidget);
}

void NotificationListenerElement::detach()
{
    _notificationListenerWidget = nullptr;
    StatelessElement::detach();
}

MultiChildElement::MultiChildElement(Object::Ref<MultiChildWidget> widget) : _multiChildWidget(widget), Element(widget) {}

void MultiChildElement::build()
{
    Object::List<Object::Ref<Widget>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children.size(); i++)
    {
        Object::Ref<Widget> &widget = children[i];
        if (i < this->_childrenElements.size()) // update widget
        {
            assert(_childrenElements[i] != nullptr && "Widget [createElement] return null isn't allowed. ");
            Object::Ref<Widget> &oldWidget = _childrenElements[i]->widget;
            if (Object::identical(widget, oldWidget))
                return;
            else if (widget->canUpdate(oldWidget))
            {
                this->_childrenElements[i]->update(widget);
                this->_childrenElements[i]->build();
            }
            else
            {
                this->_childrenElements[i]->detach();
                this->_childrenElements[i] = widget->createElement();
                assert(this->_childrenElements[i] != nullptr);
                this->_childrenElements[i]->parent = Object::cast<MultiChildElement>(this);
                this->_childrenElements[i]->attach();
                this->_childrenElements[i]->build();
            }
        }
        else // append widget
        {
            this->_childrenElements.push_back(widget->createElement());
            assert(this->_childrenElements[i] != nullptr);
            this->_childrenElements[i]->parent = Object::cast<MultiChildElement>(this);
            this->_childrenElements[i]->attach();
            this->_childrenElements[i]->build();
        }
    }
    if (this->_childrenElements.size() > children.size()) // remove widget
    {
        size_t redundant = this->_childrenElements.size() - children.size();
        for (size_t i = 0; i < redundant; i++)
        {
            auto iter = _childrenElements.rbegin();
            Object::Ref<Element> element = *iter;
            element->detach();
            this->_childrenElements.pop_back();
        }
    }
}

void MultiChildElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<MultiChildWidget> castedWidget = newWidget->cast<MultiChildWidget>();
    assert(castedWidget != nullptr);
    this->_multiChildWidget = castedWidget;
    this->widget = newWidget;
}

void MultiChildElement::notify()
{
    Element::notify();
    for (size_t i = 0; i < this->_childrenElements.size(); i++)
        this->_childrenElements[i]->notify();
}

void MultiChildElement::detach()
{
    for (size_t i = 0; i < this->_childrenElements.size(); i++)
        this->_childrenElements[i]->detach();
    this->_childrenElements.clear();
    this->_multiChildWidget = nullptr;
    Element::detach();
}