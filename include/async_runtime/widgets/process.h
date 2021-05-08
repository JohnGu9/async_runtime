#pragma once

#include "inherited_widget.h"
#include "../basic/value_notifier.h"

class RootElement;

class Command : public ValueNotifier<ref<List<ref<String>>>>
{
    using super = ValueNotifier<ref<List<ref<String>>>>;

public:
    Command() : ValueNotifier<ref<List<ref<String>>>>(Object::create<List<ref<String>>>()),
                _commandListeners(Object::create<Set<Function<void(ref<List<ref<String>>>)>>>())
    {
        addListener([this] {
            for (const auto &listener : _commandListeners)
                listener(_value);
        });
    }

    void setValue(const ref<List<ref<String>>> &value) override
    {
        this->_value = value;
        notifyListeners();
    }

    void setValue(ref<List<ref<String>>> &&value) override
    {
        this->_value = value;
        notifyListeners();
    }

    void addListener(Function<void(ref<List<ref<String>>>)> fn) { _commandListeners->insert(fn); }
    void removeListener(Function<void(ref<List<ref<String>>>)> fn) { _commandListeners->erase(_commandListeners->find(fn)); }

protected:
    ref<Set<Function<void(ref<List<ref<String>>>)>>> _commandListeners;
    void addListener(Function<void()> fn) override { super::addListener(fn); }
    void removeListener(Function<void()> fn) override { super::removeListener(fn); }
};

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
    finalref<Command> &command;

protected:
    weakref<RootElement> _root;
};