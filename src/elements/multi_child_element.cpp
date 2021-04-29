#include "async_runtime/elements/multi_child_element.h"
#include "async_runtime/widgets/multi_child_widget.h"

// MultiChild Element
MultiChildElement::MultiChildElement(ref<MultiChildWidget> widget)
    : Element(widget), _multiChildWidget(widget), _childrenElements(Object::create<List<ref<Element>>>()) {}

void MultiChildElement::attach()
{
    Element::attach();
    ref<List<ref<Widget>>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children->size(); i++)
    {
        ref<Widget> &widget = children[i];
        this->_childrenElements->emplace_back(widget->createElement());
        ref<Element> &childElement = this->_childrenElements[i];
        childElement->parent = self();
        childElement->attach();
    }
}

void MultiChildElement::build()
{
    ref<List<ref<Widget>>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children->size(); i++)
    {
        ref<Widget> &widget = children[i];
        if (i < this->_childrenElements->size()) // update widget
        {
            ref<Widget> &oldWidget = _childrenElements[i]->widget;
            if (Object::identical(widget, oldWidget))
                return;
            else if (widget->canUpdate(oldWidget))
                this->_childrenElements[i]->update(widget);
            else
            {
                this->_childrenElements[i]->detach();
                this->_childrenElements[i] = widget->createElement();
                this->_childrenElements[i]->parent = self();
                this->_childrenElements[i]->attach();
            }
        }
        else // append widget
        {
            this->_childrenElements->emplace_back(widget->createElement());
            ref<Element> &childElement = this->_childrenElements[i];
            childElement->parent = self();
            childElement->attach();
        }
    }
    if (this->_childrenElements->size() > children->size()) // remove widget
    {
        size_t redundant = this->_childrenElements->size() - children->size();
        for (size_t i = 0; i < redundant; i++)
        {
            auto iter = this->_childrenElements->rbegin();
            ref<Element> element = *iter;
            element->detach();
            this->_childrenElements->pop_back();
        }
    }
}

void MultiChildElement::update(ref<Widget> newWidget)
{
    this->_multiChildWidget = newWidget->covariant<MultiChildWidget>();
    Element::update(newWidget);
}

void MultiChildElement::notify(ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_multiChildWidget = newWidget->covariant<MultiChildWidget>();

    ref<List<ref<Widget>>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children->size(); i++)
    {
        ref<Widget> &widget = children[i];
        if (i < this->_childrenElements->size()) // update widget
        {
            ref<Element> &element = this->_childrenElements[i];
            ref<Widget> &oldWidget = _childrenElements[i]->widget;
            if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
                element->notify(widget);
            else
            {
                element->detach();
                this->_childrenElements[i] = widget->createElement();
                element = this->_childrenElements[i];
                element->parent = self();
                element->attach();
            }
        }
        else // append widget
        {
            this->_childrenElements->emplace_back(widget->createElement());
            ref<Element> &childElement = this->_childrenElements[i];
            childElement->parent = self();
            childElement->attach();
        }
    }
    if (this->_childrenElements->size() > children->size()) // remove widget
    {
        size_t redundant = this->_childrenElements->size() - children->size();
        for (size_t i = 0; i < redundant; i++)
        {
            auto iter = _childrenElements->rbegin();
            (*iter)->detach();
            this->_childrenElements->pop_back();
        }
    }
}

void MultiChildElement::detach()
{
    for (size_t i = 0; i < this->_childrenElements->size(); i++)
        this->_childrenElements[i]->detach();
    Object::detach(this->_childrenElements);
    Element::detach();
}

void MultiChildElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    for (size_t i = 0; i < this->_childrenElements->size(); i++)
    {
        if (fn(this->_childrenElements[i]) == false)
            this->_childrenElements[i]->visitDescendant(fn);
    }
}
