#pragma once

#include "inherited_widget.h"

class RootElement;
class Process : public InheritedWidget
{
public:
    static ref<Process> of(ref<BuildContext> context);
    Process(ref<Widget> child, ref<RootElement> root, option<Key> key = nullptr)
        : InheritedWidget(child, key), _root(root) {}

    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;
    virtual void exit();

protected:
    weakref<RootElement> _root;
};