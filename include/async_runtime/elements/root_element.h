#pragma once

#include <atomic>
#include "key.h"
#include "single_child_element.h"
#include "../fundamental/async.h"

class GlobalKey;
class LoggerHandler;
class ThreadPool;
class RootElement : public SingleChildElement
{
    friend class Process;
    friend class _RootFundamentalState;

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

    lateref<AsyncStreamController<ref<String>>> _command;
    ref<Widget> _child;
    lateref<GlobalKey> _coutKey;
    std::atomic_bool _consoleStop;
    int _exitCode = 0;

public:
    const int &exitCode = _exitCode;
};
