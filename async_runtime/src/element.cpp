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
    this->widget = nullptr;
}

void Element::update(Object::Ref<Widget> newWidget)
{
    this->widget = newWidget;
    this->build();
}

void Element::notify(Object::Ref<Widget> newWidget)
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    this->_inheritances = parent->_inheritances; // update inheritances
}

void Element::visitAncestor(Fn<bool(Object::Ref<Element>)> fn)
{
    Object::Ref<Element> parent = this->parent.lock();
    assert(parent != nullptr && "Element can't find out its parent. ");
    if (fn(parent) == false)
        parent->visitAncestor(fn);
}

/// Leaf Element
LeafElement::LeafElement(Object::Ref<LeafWidget> widget) : Element(widget) {}

void LeafElement::build() {}

void LeafElement::notify(Object::Ref<Widget> newWidget) {}

void LeafElement::update(Object::Ref<Widget> newWidget) {}

void LeafElement::visitDescendant(Fn<bool(Object::Ref<Element>)>) {}

/// Root Element
RootElement::RootElement(Object::Ref<Widget> widget) : Element(nullptr), _child(widget) {}

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(Object::Ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    Object::Ref<Widget> widget = _child;
    assert(widget != nullptr);
    this->_childElement = widget->createElement();
    assert(this->_childElement != nullptr);
    this->_childElement->parent = Object::cast<RootElement>(this);
    this->_childElement->attach();
}

void RootElement::build()
{
    assert(this->_childElement != nullptr);
    Object::Ref<Widget> widget = this->_child;
    assert(widget != nullptr);
    Object::Ref<Widget> lastWidget = this->_childElement->widget;
    if (Object::identical(widget, lastWidget))
        return;
    else if (widget->canUpdate(lastWidget))
        this->_childElement->update(widget);
    else
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr);
        this->_childElement->parent = Object::cast<RootElement>(this);
        this->_childElement->attach();
    }
}

void RootElement::detach()
{
    assert(this->_childElement != nullptr);
    this->_childElement->detach();
    this->_childElement = nullptr;
    Element::detach();
}

void RootElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    assert(this->_childElement != nullptr);
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void RootElement::visitAncestor(Fn<bool(Object::Ref<Element>)>) {}

/// Inherited Element
InheritedElement::InheritedElement(Object::Ref<InheritedWidget> widget) : _inheritWidget(widget), Element(widget) {}

void InheritedElement::attach()
{
    Element::attach();
    this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    this->_childElement = widget->createElement();
    assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
    this->_childElement->parent = Object::cast<InheritedElement>(this);
    this->_childElement->attach();
}

void InheritedElement::detach()
{
    assert(this->_childElement != nullptr);
    this->_childElement->detach();
    this->_childElement = nullptr;
    this->_inheritWidget = nullptr;
    Element::detach();
}

void InheritedElement::build()
{
    assert(this->_childElement != nullptr);
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<InheritedElement>(this);
        this->_childElement->attach();
    }
}

void InheritedElement::update(Object::Ref<Widget> newWidget)
{
    Object::Ref<InheritedWidget> castedWidget = newWidget->cast<InheritedWidget>();
    assert(castedWidget != nullptr);
    Object::Ref<InheritedWidget> oldWidget = this->_inheritWidget;
    this->_inheritWidget = castedWidget;
    if (castedWidget->updateShouldNotify(oldWidget))
    {
        this->widget = newWidget;
        this->_inheritances[this->_inheritWidget->runtimeType()] = Object::cast<Inheritance>(this->_inheritWidget.get());
        this->notify(nullptr);
    }
    else
        Element::update(newWidget);
}

void InheritedElement::notify(Object::Ref<Widget> newWidget)
{
    if (newWidget != nullptr)
    {
        Object::Ref<InheritedWidget> castedWidget = newWidget->cast<InheritedWidget>();
        assert(castedWidget != nullptr);
        Object::Ref<InheritedWidget> oldWidget = this->_inheritWidget;
        this->widget = newWidget;
        this->_inheritWidget = castedWidget;
    }

    assert(this->_childElement != nullptr);
    Object::Ref<Widget> widget = this->_inheritWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<InheritedElement>(this);
        this->_childElement->attach();
    }
}

void InheritedElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

/// Stateless Element
StatelessElement::StatelessElement(Object::Ref<StatelessWidget> widget) : _statelessWidget(widget), Element(widget) {}

void StatelessElement::attach()
{
    Element::attach();
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
    this->_childElement = widget->createElement();
    assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
    this->_childElement->parent = Object::cast<StatelessElement>(this);
    this->_childElement->attach();
}

void StatelessElement::detach()
{
    assert(this->_childElement != nullptr);
    this->_childElement->detach();
    this->_childElement = nullptr;
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
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<StatelessElement>(this);
        this->_childElement->attach();
    }
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
    assert(this->_childElement != nullptr);
    this->_statelessWidget = newWidget->cast<StatelessWidget>();
    assert(this->_statelessWidget);
    this->widget = newWidget;
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<StatelessElement>(this);
        this->_childElement->attach();
    }
}

void StatelessElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
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
    assert(this->_state->mounted == false && "This [State] class mount twice is not allowed. User should not reuse [State] class or manually call [initState]");
    this->_state->element = Object::cast<StatefulElement>(this);
    this->_state->initState();
    this->_state->mounted = true;
    this->_state->didDependenceChanged();

    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    this->_childElement = widget->createElement();
    assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
    this->_childElement->parent = Object::cast<StatefulElement>(this);
    this->_childElement->attach();
}

void StatefulElement::detach()
{
    assert(this->_childElement != nullptr);
    assert(this->_state->mounted && "This [State] class dispose more than twice is not allowed. User should not reuse [State] class or manually call [dispose]");
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
    assert(this->_childElement != nullptr);
    assert(this->_state->mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget))
        return;
    else if (widget->canUpdate(oldWidget))
        this->_childElement->update(widget);
    else
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<StatefulElement>(this);
        this->_childElement->attach();
    }
}

void StatefulElement::update(Object::Ref<Widget> newWidget)
{
    assert(this->_state->mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
    Object::Ref<StatefulWidget> castedWidget = newWidget->cast<StatefulWidget>();
    assert(castedWidget != nullptr);
    Object::Ref<StatefulWidget> oldWidget = this->_statefulWidget;
    this->_statefulWidget = castedWidget;
    this->_state->didWidgetUpdated(oldWidget);
    Element::update(newWidget);
}

void StatefulElement::notify(Object::Ref<Widget> newWidget)
{
    Element::notify(newWidget);
    assert(this->_childElement != nullptr);
    {
        assert(this->_state->mounted && "This [State] class has been disposed. User should not reuse [State] class or manually call [dispose]");
        Object::Ref<StatefulWidget> castedWidget = newWidget->cast<StatefulWidget>();
        assert(castedWidget != nullptr);
        Object::Ref<StatefulWidget> oldWidget = this->_statefulWidget;
        this->widget = newWidget;
        this->_statefulWidget = castedWidget;
        this->_state->didWidgetUpdated(oldWidget);
    }
    this->_state->didDependenceChanged();
    Object::Ref<Widget> widget = this->_state->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "State build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
    {
        this->_childElement->detach();
        this->_childElement = widget->createElement();
        assert(this->_childElement != nullptr && "Widget [createElement] return null isn't allowed. ");
        this->_childElement->parent = Object::cast<StatefulElement>(this);
        this->_childElement->attach();
    }
}

void StatefulElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

// NotificationListener Element
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

// MultiChild Element
MultiChildElement::MultiChildElement(Object::Ref<MultiChildWidget> widget) : _multiChildWidget(widget), Element(widget) {}

void MultiChildElement::attach()
{
    Element::attach();
    Object::List<Object::Ref<Widget>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children.size(); i++)
    {
        Object::Ref<Widget> &widget = children[i];
        this->_childrenElements.push_back(widget->createElement());
        Object::Ref<Element> &childElement = this->_childrenElements[i];
        assert(childElement != nullptr);
        childElement->parent = Object::cast<MultiChildElement>(this);
        childElement->attach();
    }
}

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
                this->_childrenElements[i]->update(widget);
            else
            {
                this->_childrenElements[i]->detach();
                this->_childrenElements[i] = widget->createElement();
                assert(this->_childrenElements[i] != nullptr);
                this->_childrenElements[i]->parent = Object::cast<MultiChildElement>(this);
                this->_childrenElements[i]->attach();
            }
        }
        else // append widget
        {
            this->_childrenElements.push_back(widget->createElement());
            Object::Ref<Element> &childElement = this->_childrenElements[i];
            assert(childElement != nullptr);
            childElement->parent = Object::cast<MultiChildElement>(this);
            childElement->attach();
        }
    }
    if (this->_childrenElements.size() > children.size()) // remove widget
    {
        size_t redundant = this->_childrenElements.size() - children.size();
        for (size_t i = 0; i < redundant; i++)
        {
            auto iter = this->_childrenElements.rbegin();
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
    Element::update(newWidget);
}

void MultiChildElement::notify(Object::Ref<Widget> newWidget)
{
    Element::notify(newWidget);
    Object::Ref<MultiChildWidget> castedWidget = newWidget->cast<MultiChildWidget>();
    assert(castedWidget != nullptr);
    this->_multiChildWidget = castedWidget;
    this->widget = newWidget;

    Object::List<Object::Ref<Widget>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children.size(); i++)
    {
        Object::Ref<Widget> &widget = children[i];
        if (i < this->_childrenElements.size()) // update widget
        {
            Object::Ref<Element> &element = this->_childrenElements[i];
            assert(element != nullptr && "Widget [createElement] return null isn't allowed. ");
            Object::Ref<Widget> &oldWidget = _childrenElements[i]->widget;
            if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
                element->notify(widget);
            else
            {
                element->detach();
                this->_childrenElements[i] = widget->createElement();
                element = this->_childrenElements[i];
                assert(element != nullptr);
                element->parent = Object::cast<MultiChildElement>(this);
                element->attach();
            }
        }
        else // append widget
        {
            this->_childrenElements.push_back(widget->createElement());
            Object::Ref<Element> &childElement = this->_childrenElements[i];
            assert(childElement != nullptr);
            childElement->parent = Object::cast<MultiChildElement>(this);
            childElement->attach();
        }
    }
    if (this->_childrenElements.size() > children.size()) // remove widget
    {
        size_t redundant = this->_childrenElements.size() - children.size();
        for (size_t i = 0; i < redundant; i++)
        {
            auto iter = _childrenElements.rbegin();
            (*iter)->detach();
            this->_childrenElements.pop_back();
        }
    }
}

void MultiChildElement::detach()
{
    for (size_t i = 0; i < this->_childrenElements.size(); i++)
        this->_childrenElements[i]->detach();
    this->_childrenElements.clear();
    this->_multiChildWidget = nullptr;
    Element::detach();
}

void MultiChildElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    for (size_t i = 0; i < this->_childrenElements.size(); i++)
    {
        if (fn(this->_childrenElements[i]) == false)
            this->_childrenElements[i]->visitDescendant(fn);
    }
}