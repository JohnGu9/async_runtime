#pragma once

#include "../widgets/inherited_widget.h"
#include "../widgets/stateful_widget.h"
#include "state_helper.h"
#include "async.h"
#include "disposable.h"

class LoggerHandler : public virtual Object, public virtual StateHelper, public virtual Disposable
{
public:
    LoggerHandler(State<StatefulWidget> *state) : _state(Object::cast<>(state)) {}

    virtual Object::Ref<Future<bool>> write(String str) = 0;
    virtual Object::Ref<Future<bool>> writeLine(String str) = 0;

protected:
    LoggerHandler() {}
    Object::Ref<State<StatefulWidget>> _state;
};

class Logger : public InheritedWidget
{
public:
    using Handler = Object::Ref<LoggerHandler>;
    static Handler of(Object::Ref<BuildContext> context);

    // support hot switch
    static Object::Ref<Widget> stdout(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    static Object::Ref<Widget> file(String path, Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    static Object::Ref<Widget> block(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);

    Logger(Object::Ref<Widget> child, Handler handler, Object::Ref<Key> key = nullptr);
    bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) override;

protected:
    Handler _handler;
};

class StdoutLogger : public StatefulWidget
{
public:
    static Logger::Handler of(Object::Ref<BuildContext> context);
    StdoutLogger(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    Object::Ref<State<StatefulWidget>> createState() override;
    Object::Ref<Widget> child;
};

class StdoutLoggerState : public State<StdoutLogger>
{
public:
    using super = State<StdoutLogger>;
    Logger::Handler _handler;

    void initState() override;
    void dispose() override;
    Object::Ref<Widget> build(Object::Ref<BuildContext>) override;
};

inline Object::Ref<State<StatefulWidget>> StdoutLogger::createState()
{
    return Object::create<StdoutLoggerState>();
}