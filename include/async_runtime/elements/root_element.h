#pragma once

#include "single_child_element.h"

class GlobalKey;
class LoggerHandler;
class RootInheritedWidget;
class RootElement : public SingleChildElement
{
    friend RootInheritedWidget;

public:
    RootElement(Object::Ref<Widget> child);

    void attach() override;
    void detach() override;
    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
    virtual void scheduleRootWidget();

    virtual Object::Ref<LoggerHandler> getStdoutHandler();
    virtual Object::Ref<ThreadPool> getMainHandler();
    virtual void onCommand(const std::string &in);

protected:
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;
    void visitAncestor(Function<bool(Object::Ref<Element>)>) override;

    std::mutex _mutex;
    std::condition_variable _condition;

    Object::Ref<Widget> _child;
    Object::Ref<GlobalKey> _stdoutKey;
    std::atomic_bool _consoleStop;
    virtual void _console();
};