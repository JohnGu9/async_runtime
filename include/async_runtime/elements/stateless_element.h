#pragma once

#include "single_child_element.h"

class StatelessWidget;
class StatelessElement : public SingleChildElement
{
public:
    StatelessElement(Object::Ref<StatelessWidget> widget);

protected:
    Object::Ref<StatelessWidget> _statelessWidget;

    void attach() override;
    void detach() override;
    void build() override;
    void notify(Object::Ref<Widget> newWidget) override;
    void update(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};