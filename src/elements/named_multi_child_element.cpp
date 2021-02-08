#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/named_multi_child_widget.h"

NamedMultiChildElement::NamedMultiChildElement(Object::Ref<NamedMultiChildWidget> widget)
    : _childrenElements({}), _namedMultiChildWidget(widget), Element(widget) {}

void NamedMultiChildElement::attach()
{
    Element::attach();
    Map<String, Object::Ref<Widget>> &children = this->_namedMultiChildWidget->_children;
    for (auto &iter : children)
    {
        Object::Ref<Widget> &widget = iter.second;
        Object::Ref<Element> element = widget->createElement();
        assert(element != nullptr);
        this->_childrenElements[iter.first] = element;
        element->parent = Object::cast<>(this);
        element->attach();
    }
}

void NamedMultiChildElement::detach()
{
    for (auto &iter : this->_childrenElements)
        iter.second->detach();
    this->_childrenElements = nullptr;
    this->_namedMultiChildWidget = nullptr;
    Element::detach();
}

void NamedMultiChildElement::build()
{
    Map<String, Object::Ref<Widget>> &children = this->_namedMultiChildWidget->_children;
    for (auto iter = this->_childrenElements->begin(); iter != this->_childrenElements->end(); iter++)
    {
        if (children->find(iter->first) == children->end())
        {
            iter->second->detach();
            iter = this->_childrenElements->erase(iter);
        }
    }

    for (auto &iter : children)
    {
        const String &key = iter.first;
        Object::Ref<Widget> &widget = iter.second;
        auto elementIterator = this->_childrenElements->find(key);

        if (elementIterator == this->_childrenElements->end())
        {
            Object::Ref<Element> newElement = widget->createElement();
            this->_childrenElements[key] = newElement;
            newElement->parent = Object::cast<>(this);
            newElement->attach();
        }
        else
        {
            Object::Ref<Element> &element = elementIterator->second;
            Object::Ref<Widget> &oldWidget = element->widget;
            if (Object::identical(widget, oldWidget))
                return;
            else if (widget->canUpdate(oldWidget))
                element->update(widget);
            else
            {
                element->detach();
                Object::Ref<Element> newElement = widget->createElement();
                assert(newElement != nullptr);
                elementIterator->second = newElement;
                elementIterator->second->parent = Object::cast<>(this);
                elementIterator->second->attach();
            }
        }
    }
}

void NamedMultiChildElement::update(Object::Ref<Widget> newWidget)
{
    this->_namedMultiChildWidget = newWidget->covariant<NamedMultiChildWidget>();
    Element::update(newWidget);
}

void NamedMultiChildElement::notify(Object::Ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_namedMultiChildWidget = newWidget->covariant<NamedMultiChildWidget>();

    Map<String, Object::Ref<Widget>> &children = this->_namedMultiChildWidget->_children;
    for (auto iter = this->_childrenElements->begin(); iter != this->_childrenElements->end(); iter++)
    {
        if (children->find(iter->first) == children->end())
        {
            iter->second->detach();
            iter = this->_childrenElements->erase(iter);
        }
    }

    for (auto &iter : children)
    {
        const String &key = iter.first;
        Object::Ref<Widget> &widget = iter.second;
        auto elementIterator = this->_childrenElements->find(key);

        if (elementIterator == this->_childrenElements->end())
        {
            Object::Ref<Element> newElement = widget->createElement();
            this->_childrenElements[key] = newElement;
            newElement->parent = Object::cast<>(this);
            newElement->attach();
        }
        else
        {
            Object::Ref<Element> &element = elementIterator->second;
            Object::Ref<Widget> &oldWidget = element->widget;
            if (widget->canUpdate(oldWidget))
                element->notify(widget);
            else
            {
                element->detach();
                Object::Ref<Element> newElement = widget->createElement();
                assert(newElement != nullptr);
                elementIterator->second = newElement;
                elementIterator->second->parent = Object::cast<>(this);
                elementIterator->second->attach();
            }
        }
    }
}

void NamedMultiChildElement::visitDescendant(Function<bool(Object::Ref<Element>)>) {}