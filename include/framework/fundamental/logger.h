#pragma once

#include <sstream>
#include "scheduler.h"

class LoggerHandler : public Object
{
public:
    LoggerHandler(Scheduler::Handler sync_) : sync(sync_) {}
    Scheduler::Handler sync; // output sync

    virtual std::future<bool> write(String str) = 0;
    virtual std::future<bool> writeLine(String str) = 0;

    virtual void dispose()
    {
        sync = nullptr;
    }
};

class Logger : public InheritedWidget
{
public:
    using Handler = Object::Ref<LoggerHandler>;
    static Handler of(Object::Ref<BuildContext> context);
    static Object::Ref<Widget> stdoutProxy(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    static Object::Ref<Widget> file(Object::Ref<Widget> child, String path, Object::Ref<Key> key = nullptr);

    Logger(Object::Ref<Widget> child, Handler handler, Object::Ref<Key> key = nullptr);
    bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) override;

protected:
    Handler _handler;
};

class _StdoutLoggerState;
class StdoutLogger : public StatefulWidget
{
    friend _StdoutLoggerState;
    Object::Ref<Widget> _child;

    Object::Ref<State> createState() override;

public:
    static Logger::Handler of(Object::Ref<BuildContext> context);
    StdoutLogger(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
};

class _StdoutLoggerState : public State<StdoutLogger>
{
public:
    using super = State<StdoutLogger>;
    Logger::Handler _handler;

    void didDependenceChanged() override;
    void dispose() override;
    Object::Ref<Widget> build(Object::Ref<BuildContext>) override;
};