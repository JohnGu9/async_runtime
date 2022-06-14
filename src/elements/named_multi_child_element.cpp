#include "async_runtime/elements/named_multi_child_element.h"
#include "async_runtime/widgets/named_multi_child_widget.h"

NamedMultiChildElement::NamedMultiChildElement(ref<NamedMultiChildWidget> widget)
    : Element(widget), _namedMultiChildWidget(widget), _childrenElements(Map<ref<String>, lateref<Element>>::create()) {}

void NamedMultiChildElement::attach()
{
    Element::attach();
    finalref<Map<ref<String>, lateref<Widget>>> &children = this->_namedMultiChildWidget->_children;
    for (auto &iter : children)
    {
        finalref<Widget> &widget = iter.second;
        ref<Element> element = widget->createElement();
        this->_childrenElements[iter.first] = element;
        element->parent = self();
        element->attach();
    }
}

void NamedMultiChildElement::detach()
{
    for (auto &iter : this->_childrenElements)
        iter.second->detach();
    this->_childrenElements->clear();
    Element::detach();
}

void NamedMultiChildElement::build()
{
    finalref<Map<ref<String>, lateref<Widget>>> &children = this->_namedMultiChildWidget->_children;
    for (auto iter = this->_childrenElements->begin(); iter != this->_childrenElements->end();)
    {
        if (children->findKey((*iter).first) == children->end())
        {
            (*iter).second->detach();
            iter = this->_childrenElements->erase(iter);
        }
        else
        {
            iter++;
        }
    }

    for (auto &iter : children)
    {
        finalref<String> &key = iter.first;
        finalref<Widget> &widget = iter.second;
        auto elementIterator = this->_childrenElements->findKey(key);

        if (elementIterator == this->_childrenElements->end())
        {
            ref<Element> newElement = widget->createElement();
            this->_childrenElements[key] = newElement;
            newElement->parent = self();
            newElement->attach();
        }
        else
        {
            ref<Element> &element = this->_childrenElements[key];
            finalref<Widget> &oldWidget = element->getWidget();
            if (Object::identical(widget, oldWidget))
                continue;
            else if (widget->canUpdate(oldWidget))
                element->update(widget);
            else
            {
                element->detach();
                ref<Element> newElement = widget->createElement();
                element = newElement;
                element->parent = self();
                element->attach();
            }
        }
    }
}

void NamedMultiChildElement::update(ref<Widget> newWidget)
{
    this->_namedMultiChildWidget = newWidget->covariant<NamedMultiChildWidget>();
    Element::update(newWidget);
}

void NamedMultiChildElement::notify(ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_namedMultiChildWidget = newWidget->covariant<NamedMultiChildWidget>();

    finalref<Map<ref<String>, lateref<Widget>>> &children = this->_namedMultiChildWidget->_children;
    for (auto iter = this->_childrenElements->begin(); iter != this->_childrenElements->end();)
    {
        if (children->findKey((*iter).first) == children->end())
        {
            (*iter).second->detach();
            iter = this->_childrenElements->erase(iter);
        }
        else
        {
            iter++;
        }
    }

    for (auto &iter : children)
    {
        finalref<String> &key = iter.first;
        finalref<Widget> &widget = iter.second;
        auto elementIterator = this->_childrenElements->findKey(key);

        if (elementIterator == this->_childrenElements->end())
        {
            ref<Element> newElement = widget->createElement();
            this->_childrenElements[key] = newElement;
            newElement->parent = self();
            newElement->attach();
        }
        else
        {
            ref<Element> &element = this->_childrenElements[key];
            finalref<Widget> &oldWidget = element->getWidget();
            if (widget->canUpdate(oldWidget))
                element->notify(widget);
            else
            {
                element->detach();
                ref<Element> newElement = widget->createElement();
                element = newElement;
                element->parent = self();
                element->attach();
            }
        }
    }
}

void NamedMultiChildElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    for (auto &iter : this->_childrenElements)
    {
        if (fn(iter.second) == false)
            iter.second->visitDescendant(fn);
    }
}
