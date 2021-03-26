#pragma once

#include "inherited_widget.h"

class RootElement;
class RootInheritedWidget : public InheritedWidget
{
public:
    static ref<RootInheritedWidget> of(ref<BuildContext> context);
    RootInheritedWidget(ref<Widget> child, ref<RootElement> root, option<Key> key = nullptr)
        : _root(root), InheritedWidget(child, key) {}

    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;
    virtual void exit();

protected:
    weakref<RootElement> _root;
};