#pragma once

#include "inherited_widget.h"
#include "../fundamental/async.h"

class RootElement;
class Process : public InheritedWidget
{
public:
    static ref<Process> of(ref<BuildContext> context);
    Process(ref<Widget> child, ref<RootElement> root, option<Key> key = nullptr);

    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;

    /// origin function is async that can not exit runApp immediately
    /// you can wrap your widget with a mock [Process] by pass a mock [RootElement] to catch the exit action
    ///
    /// example:
    /// class MockRootElement : public RootElement {
    ///     void _exit() override { /* my code */}
    /// };
    ///
    /// // Process do not lock the ref<RootElement> resource, you need to catch the resource by your own
    /// Object::create<Process>(${yourWidget}, ${mockRootElement});
    ///
    /// Alarm: only affect runApp return value, not exit program directly!
    /// AsyncRuntime don't change program's life-cycle. It's always gentlemanly handling anything.
    virtual void exit(const int exitCode = 0);
    finalref<Stream<ref<String>>> &command;

protected:
    weakref<RootElement> _root;
};