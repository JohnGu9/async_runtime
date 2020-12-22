#pragma once

#include "element.h"

class Widget;
class StatefulWidget;

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type * = nullptr>
class State;

class StatefulElement : public virtual Element
{

public:
    StatefulElement(Object::Ref<StatefulWidget> widget);

    Object::Ref<StatefulWidget> _statefulWidget;
    Object::Ref<State<StatefulWidget>> _state;

protected:
    Object::Ref<Element> _childElement;

    virtual void build() override;
    virtual void notify() override;
    virtual void attach() override;
    virtual void detach() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
};