#pragma once

#include "../widgets/inherited_widget.h"
#include "../widgets/stateful_widget.h"
#include "state_helper.h"
#include "async.h"
#include "disposable.h"

class LoggerHandler : public virtual Object, public StateHelper, public Disposable
{
public:
    LoggerHandler(State<StatefulWidget> *state) : _state(Object::cast<>(state)) {}

    virtual ref<Future<bool>> write(String str) = 0;
    virtual ref<Future<bool>> writeLine(String str) = 0;

protected:
    LoggerHandler() {}
    lateref<State<StatefulWidget>> _state;
};

class Logger : public InheritedWidget
{
public:
    using Handler = ref<LoggerHandler>;
    static Handler of(ref<BuildContext> context);

    // support hot switch
    static ref<Widget> stdout(ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> file(String path, ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> block(ref<Widget> child, option<Key> key = nullptr);

    Logger(ref<Widget> child, Handler handler, option<Key> key = nullptr);
    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;

protected:
    Handler _handler;
};

class StdoutLogger : public StatefulWidget
{
public:
    static Logger::Handler of(ref<BuildContext> context);
    StdoutLogger(ref<Widget> child, option<Key> key = nullptr);
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
};

class StdoutLoggerState : public State<StdoutLogger>
{
public:
    using super = State<StdoutLogger>;
    lateref<LoggerHandler> _handler;

    void initState() override;
    void dispose() override;
    ref<Widget> build(ref<BuildContext>) override;
};

inline ref<State<StatefulWidget>> StdoutLogger::createState()
{
    return Object::create<StdoutLoggerState>();
}