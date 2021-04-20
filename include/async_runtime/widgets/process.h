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
    virtual void exit(const int exitCode = 0
                      /* alarm: only affect runApp return value, not exit program directly! */
                      /* AsyncRuntime don't change program's life-cycle. It's always gentlemanly handling anything. */
                      );

protected:
    weakref<RootElement> _root;
};