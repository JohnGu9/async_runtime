#pragma once

#include "key.h"
#include "stateful_widget.h"

template <class TargetWidget = StatefulWidget, class TargetState = State<TargetWidget>>
class GlobalKey : public Key
{
public:
    bool isEqual(option<Key> other) override
    {
        return Object::identical<>(self(), other);
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
    RUNTIME_ASSERT(this->_element.toOption() == nullptr, "One [GlobalKey] instance can't mount more than one element. ");
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
    option<Element> element = this->_element.toOption();
    if_not_null(element) return element->getWidget()->template covariant<TargetWidget>();
    end_if() return nullptr;
}

template <class TargetWidget, class TargetState>
option<BuildContext> GlobalKey<TargetWidget, TargetState>::getCurrentContext() { return this->_element.toOption(); }

template <class TargetWidget, class TargetState>
option<TargetState> GlobalKey<TargetWidget, TargetState>::getCurrentState()
{
    option<Element> element = this->_element.toOption();
    if_not_null(element)
    {
        option<StatefulElement> statefulElement = element->cast<StatefulElement>();
        if_not_null(statefulElement) return statefulElement->_state->template covariant<TargetState>();
        end_if()
    }
    end_if();
    return nullptr;
}

template <class TargetWidget = StatefulWidget, class TargetState = State<TargetWidget>>
class GlobalObjectKey : public GlobalKey<TargetWidget, TargetState>
{
public:
    GlobalObjectKey(option<Object> object) : _object(object){};
    bool isEqual(option<Key> other) override
    {
        if (auto ptr = dynamic_cast<GlobalObjectKey<TargetWidget, TargetState> *>(other.get()))
        {
            return this->_object == ptr->_object;
        }
        return false;
    }

protected:
    GlobalObjectKey() = delete;
    option<Object> _object;
};
