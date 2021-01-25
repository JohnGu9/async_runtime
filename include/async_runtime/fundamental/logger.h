#pragma once

#include <sstream>
#include "../basic/state.h"
#include "scheduler.h"
#include "dispatcher.h"

class LoggerHandler : public Dispatcher
{
public:
    LoggerHandler(State<StatefulWidget> *state) : Dispatcher(state, nullptr, 0) {}

    virtual std::future<bool> write(String str) = 0;
    virtual std::future<bool> writeLine(String str) = 0;
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

class LoggerBlocker : public StatefulWidget
{
public:
    LoggerBlocker(Object::Ref<Widget> child, bool blocking = true, Object::Ref<Key> key = nullptr)
        : child(child), blocking(blocking), StatefulWidget(key) {}

    Object::Ref<Widget> child;
    bool blocking;

    Object::Ref<State<StatefulWidget>> createState() override;
};

class StdoutLogger : public StatefulWidget
{
public:
    static Logger::Handler of(Object::Ref<BuildContext> context);
    StdoutLogger(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    Object::Ref<State<StatefulWidget>> createState() override;
    Object::Ref<Widget> child;
};

class _StdoutLoggerState : public State<StdoutLogger>
{
public:
    using super = State<StdoutLogger>;
    Logger::Handler _handler;

    void initState() override;
    void dispose() override;
    Object::Ref<Widget> build(Object::Ref<BuildContext>) override;
};