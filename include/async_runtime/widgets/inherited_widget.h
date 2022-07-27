#pragma once
#include "stateless_widget.h"

class InheritedWidget : public StatelessWidget
{
public:
    InheritedWidget(ref<Widget> child, option<Key> key = nullptr);
    virtual bool updateShouldNotify(ref<InheritedWidget> oldWidget) = 0;
    ref<Widget> build(ref<BuildContext> context) override;
    ref<Element> createElement() override;

protected:
    finalref<Widget> _child;
};
