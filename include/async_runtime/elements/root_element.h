#pragma once

#include "single_child_element.h"

class GlobalKey;
class LoggerHandler;
class Process;
class RootElement : public SingleChildElement
{
    friend Process;

public:
    RootElement(ref<Widget> child);

    void attach() override;
    void detach() override;
    void visitDescendant(Function<bool(ref<Element>)>) override;
    virtual void scheduleRootWidget();

    virtual ref<LoggerHandler> getStdoutHandler();
    virtual ref<ThreadPool> getMainHandler();
    virtual void onCommand(const std::string &in);

protected:
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;
    void visitAncestor(Function<bool(ref<Element>)>) override;

    std::mutex _mutex;
    std::condition_variable _condition;

    lateref<Widget> _child;
    lateref<GlobalKey> _coutKey;
    std::atomic_bool _consoleStop;
    virtual void _console();
};