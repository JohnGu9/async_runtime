#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/multi_child_widget.h"

// MultiChild Element
MultiChildElement::MultiChildElement(Object::Ref<MultiChildWidget> widget) : _multiChildWidget(widget), Element(widget) {}

void MultiChildElement::attach()
{
    Element::attach();
    List<Object::Ref<Widget>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children.size(); i++)
    {
        Object::Ref<Widget> &widget = children[i];
        this->_childrenElements.push_back(widget->createElement());
        Object::Ref<Element> &childElement = this->_childrenElements[i];
        assert(childElement != nullptr);
        childElement->parent = Object::cast<>(this);
        childElement->attach();
    }
}

void MultiChildElement::build()
{
    List<Object::Ref<Widget>> &children = this->_multiChildWidget->_children;
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
                this->_childrenElements[i]->parent = Object::cast<>(this);
                this->_childrenElements[i]->attach();
            }
        }
        else // append widget
        {
            this->_childrenElements.push_back(widget->createElement());
            Object::Ref<Element> &childElement = this->_childrenElements[i];
            assert(childElement != nullptr);
            childElement->parent = Object::cast<>(this);
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

    List<Object::Ref<Widget>> &children = this->_multiChildWidget->_children;
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
                element->parent = Object::cast<>(this);
                element->attach();
            }
        }
        else // append widget
        {
            this->_childrenElements.push_back(widget->createElement());
            Object::Ref<Element> &childElement = this->_childrenElements[i];
            assert(childElement != nullptr);
            childElement->parent = Object::cast<>(this);
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

void MultiChildElement::visitDescendant(Function<bool(Object::Ref<Element>)> fn)
{
    for (size_t i = 0; i < this->_childrenElements.size(); i++)
    {
        if (fn(this->_childrenElements[i]) == false)
            this->_childrenElements[i]->visitDescendant(fn);
    }
}
