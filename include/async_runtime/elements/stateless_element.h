#pragma once

#include "single_child_element.h"

class StatelessWidget;
class StatelessElement : public SingleChildElement
{
public:
    StatelessElement(ref<StatelessWidget> widget);

protected:
    ref<StatelessWidget> _statelessWidget;

    void attach() override;
    void detach() override;
    void build() override;
    void notify(ref<Widget> newWidget) override;
    void update(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;
};