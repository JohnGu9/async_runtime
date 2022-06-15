#include "async_runtime/elements/multi_child_element.h"
#include "async_runtime/widgets/multi_child_widget.h"

// MultiChild Element
MultiChildElement::MultiChildElement(ref<MultiChildWidget> widget)
    : Element(widget), _multiChildWidget(widget), _childrenElements(List<ref<Element>>::create()) {}

void MultiChildElement::attach()
{
    Element::attach();
    finalref<List<ref<Widget>>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children->size(); i++)
    {
        ref<Widget> &widget = children[i];
        ref<Element> childElement = widget->createElement();
        this->_childrenElements->emplaceBack(childElement);
        childElement->parent = self();
        childElement->attach();
    }
}

void MultiChildElement::build()
{
    finalref<List<ref<Widget>>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children->size(); i++)
    {
        ref<Widget> &widget = children[i];
        if (i < this->_childrenElements->size()) // update widget
        {
            finalref<Widget> &oldWidget = _childrenElements[i]->getWidget();
            if (Object::identical(widget, oldWidget))
                continue;
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
            ref<Element> childElement = widget->createElement();
            this->_childrenElements->emplaceBack(childElement);
            childElement->parent = self();
            childElement->attach();
        }
    }
    if (this->_childrenElements->size() > children->size()) // remove widget
    {
        size_t redundant = this->_childrenElements->size() - children->size();
        for (size_t i = 0; i < redundant; i++)
        {
            ref<Element> element = this->_childrenElements[this->_childrenElements->size() - 1];
            element->detach();
            this->_childrenElements->popBack();
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

    finalref<List<ref<Widget>>> &children = this->_multiChildWidget->_children;
    for (size_t i = 0; i < children->size(); i++)
    {
        ref<Widget> &widget = children[i];
        if (i < this->_childrenElements->size()) // update widget
        {
            ref<Element> &element = this->_childrenElements[i];
            finalref<Widget> &oldWidget = _childrenElements[i]->getWidget();
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
            ref<Element> childElement = widget->createElement();
            this->_childrenElements->emplaceBack(childElement);
            childElement->parent = self();
            childElement->attach();
        }
    }
    if (this->_childrenElements->size() > children->size()) // remove widget
    {
        size_t redundant = this->_childrenElements->size() - children->size();
        for (size_t i = 0; i < redundant; i++)
        {
            ref<Element> element = this->_childrenElements[_childrenElements->size()];
            element->detach();
            this->_childrenElements->popBack();
        }
    }
}

void MultiChildElement::detach()
{
    for (size_t i = 0; i < this->_childrenElements->size(); i++)
        this->_childrenElements[i]->detach();
    this->_childrenElements->clear();
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
