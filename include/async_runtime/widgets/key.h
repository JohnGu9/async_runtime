#pragma once

#include "../object/object.h"

class Element;
class BuildContext;

class Key : public virtual Object
{
public:
    virtual bool equal(option<Key> other) = 0;
    virtual void setElement(ref<Element> element) {}
    virtual void dispose() {}
};

template <typename T>
class ValueKey : public Key
{
public:
    ValueKey(const T &value) : _value(value) {}
    ValueKey(T &&value) : _value(std::move(value)) {}

    bool equal(option<Key> other) override
    {
        lateref<Key> nonNullOther;
        if (other.isNotNull(nonNullOther))
        {
            lateref<ValueKey<T>> castedPointer;
            if (nonNullOther->cast<ValueKey<T>>().isNotNull(castedPointer))
            {
                return this->_value == castedPointer->_value;
            }
        }
        return false;
    }

protected:
    T _value;
};

#include "stateful_widget.h"

template <class TargetWidget = StatefulWidget, class TargetState = State<StatefulWidget>>
class GlobalKey : public Key
{
public:
    bool equal(option<Key> other) override
    {
        lateref<Key> key;
        if (other.isNotNull(key))
        {
            return Object::identical<>(self(), key);
        }
        else
        {
            return false;
        }
    }

    void setElement(ref<Element> element) override;
    void dispose() override;

    virtual option<TargetWidget> getCurrentWidget();
    virtual option<BuildContext> getCurrentContext();
    virtual option<TargetState> getCurrentState();

protected:
    weakref<Element> _element;
};

// [GlobalKey] is the [Key] only can be bind one widget at one time
// but you can access target widget, context and state (if widget is StatefulWidget)
template <class TargetWidget, class TargetState>
void GlobalKey<TargetWidget, TargetState>::setElement(ref<Element> element)
{
    assert(this->_element.toOption() == nullptr && "One [GlobalKey] instance can't mount more than one element. ");
    this->_element = element;
}

template <class TargetWidget, class TargetState>
void GlobalKey<TargetWidget, TargetState>::dispose()
{
    this->_element = nullptr;
}

#include "../elements/stateful_element.h"

template <class TargetWidget, class TargetState>
option<TargetWidget> GlobalKey<TargetWidget, TargetState>::getCurrentWidget()
{
    lateref<Element> element;
    if (this->_element.isNotNull(element))
        return element->getWidget()->template covariant<TargetWidget>();
    return nullptr;
}

template <class TargetWidget, class TargetState>
option<BuildContext> GlobalKey<TargetWidget, TargetState>::getCurrentContext() { return this->_element.toOption(); }

template <class TargetWidget, class TargetState>
option<TargetState> GlobalKey<TargetWidget, TargetState>::getCurrentState()
{
    lateref<Element> element;
    if (this->_element.isNotNull(element))
    {
        lateref<StatefulElement> statefulElement;
        if (element->cast<StatefulElement>().isNotNull(statefulElement))
            return statefulElement->_state->template covariant<TargetState>();
    }
    return nullptr;
}

template <class TargetWidget = StatefulWidget, class TargetState = State<StatefulWidget>>
class GlobalObjectKey : public GlobalKey<TargetWidget, TargetState>
{
public:
    GlobalObjectKey(option<Object> object) : _object(object){};
    bool equal(option<Key> other) override
    {
        lateref<Key> nonNullOther;
        if (other.isNotNull(nonNullOther))
        {
            lateref<GlobalObjectKey<TargetWidget, TargetState>> castedPointer;
            if (nonNullOther->cast<GlobalObjectKey<TargetWidget, TargetState>>().isNotNull(castedPointer))
                return Object::identical<>(this->_object, castedPointer->_object);
        }
        return false;
    }

protected:
    GlobalObjectKey() = delete;
    option<Object> _object;
};
