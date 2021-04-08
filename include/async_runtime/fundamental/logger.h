#pragma once

#include "../widgets/inherited_widget.h"
#include "../widgets/stateful_widget.h"
#include "state_helper.h"
#include "async.h"
#include "disposable.h"

/// Create custom logger [MyLoggerHandler] example:
///
/// class MyLoggerHandler : public LoggerHandler
/// {
/// public:
///     ref<Future<bool>> write(ref<String> str) override { / **** / }
///     ref<Future<bool>> writeLine(ref<String> str) override { / **** / }
/// }
///
///
/// Wrap your widget with [Logger]
///
/// Object::create<Logger>(
///     [YourWidget], // your widget to wrap
///     Object::create<MyLoggerHandler>(), // put your handler into [Logger]
/// );
///
///
/// Get [MyLoggerHandler] from [BuildContext]
///
/// ref<Widget> build(ref<BuildContext> context) override
/// {
///     ref<LoggerHandler> myLoggerHandler = Logger::of(context);
///     myLoggerHandler->writeLine("get MyLoggerHandler");
///
///     / **** /
///
///     return [YourWidget];
/// }
///

class LoggerHandler : public virtual Object, public Disposable
{
public:
    LoggerHandler() {}

    virtual ref<Future<bool>> write(ref<String> str) = 0;
    virtual ref<Future<bool>> writeLine(ref<String> str) = 0;

};

class Logger : public InheritedWidget
{
public:
    using Handler = ref<LoggerHandler>;
    static Handler of(ref<BuildContext> context);

    // support hot switch
    static ref<Widget> cout(ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> file(ref<String> path, ref<Widget> child, option<Key> key = nullptr);
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