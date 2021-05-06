#pragma once

#include "single_child_element.h"
#include "../basic/value_notifier.h"

class Command : public ValueNotifier<ref<List<ref<String>>>>
{
public:
    Command() : ValueNotifier<ref<List<ref<String>>>>(Object::create<List<ref<String>>>()),
                _commandListeners(Object::create<Set<Function<void(ref<List<ref<String>>>)>>>())
    {
        addListener([this](ref<Listenable> self) {
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

    void addListener(Function<void(ref<Listenable>)> fn) override { ValueNotifier<ref<List<ref<String>>>>::addListener(fn); }
    void removeListener(Function<void(ref<Listenable>)> fn) override { ValueNotifier<ref<List<ref<String>>>>::removeListener(fn); }

    void addListener(Function<void(ref<List<ref<String>>>)> fn) { _commandListeners->insert(fn); }
    void removeListener(Function<void(ref<List<ref<String>>>)> fn) { _commandListeners->erase(_commandListeners->find(fn)); }

protected:
    ref<Set<Function<void(ref<List<ref<String>>>)>>> _commandListeners;
};

class GlobalKey;
class LoggerHandler;
class RootElement : public SingleChildElement
{
    friend class Process;

public:
    RootElement(ref<Widget> child);

    void attach() override;
    void detach() override;
    void visitDescendant(Function<bool(ref<Element>)>) override;
    virtual void _console();
    virtual void _noConsole();

    virtual ref<LoggerHandler> getStdoutHandler();
    virtual ref<ThreadPool> getMainHandler();
    virtual void onCommand(const ref<String> &in);

protected:
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;
    void visitAncestor(Function<bool(ref<Element>)>) override;
    virtual void _exit(int exitCode);

    std::mutex _mutex;
    std::condition_variable _condition;

    lateref<Command> _command;
    lateref<Widget> _child;
    lateref<GlobalKey> _coutKey;
    std::atomic_bool _consoleStop;
    int _exitCode = 0;

public:
    const int &exitCode = _exitCode;
};
