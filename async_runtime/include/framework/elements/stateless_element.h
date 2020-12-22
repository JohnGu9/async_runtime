#pragma once

#include "element.h"

class Widget;
class StatelessWidget;

class StatelessElement : public virtual Element
{

public:
    StatelessElement(Object::Ref<StatelessWidget> widget);

protected:
    Object::Ref<StatelessWidget> _statelessWidget;
    Object::Ref<Element> _childElement;

    virtual void build() override;
    virtual void notify() override;
    virtual void detach() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
};