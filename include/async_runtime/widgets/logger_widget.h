/**
 * @brief
 *
 * @example
 * // Create custom logger [MyLoggerHandler:
 * class MyLoggerHandler : public LoggerHandler
 * {
 * public:
 *      ref<Future<bool>> write(ref<String> str) override { / **** / }
 * }
 *
 *
 *  // Wrap your widget with [Logger]
 *  Object::create<Logger>(
 *      [YourWidget], // your widget to wrap
 *      Object::create<MyLoggerHandler>(), // put your handler into [Logger]
 *  );
 *
 *
 *  // Get [MyLoggerHandler] from [BuildContext]
 *  ref<Widget> build(ref<BuildContext> context) override
 *  {
 *      ref<LoggerHandler> myLoggerHandler = Logger::of(context);
 *      myLoggerHandler->writeLine("get MyLoggerHandler");
 *
 *      / **** /
 *
 *      return ${yourWidget};
 *  }
 *
 *  Use Logger macros [LogDebug, LogInfo, LogWarning, LogError] (define in file async_runtime.h)
 *  macro support two format [printf style] [istream style]
 *  [printf style]: LogDebug("Omg {}! {}.", "args", 26); // the "{}" will be filled with arguments
 *
 *  both output same stuff
 *
 */
#pragma once
#include "../fundamental/logger.h"
#include "inherited_widget.h"
#include "stateful_widget.h"

class Logger : public InheritedWidget
{
    class _Logger;
    friend class StdoutLoggerState;

public:
    static ref<LoggerHandler> of(ref<BuildContext> context);

    // support hot switch
    static ref<Widget> cout(ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> file(ref<String> path, ref<Widget> child, option<Key> key = nullptr);
    static ref<Widget> block(ref<Widget> child, option<Key> key = nullptr);

    Logger(ref<Widget> child, ref<LoggerHandler> handler, option<Key> key = nullptr);
    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;

protected:
    ref<LoggerHandler> _handler;
};

class StdoutLogger : public StatefulWidget
{
public:
    static ref<LoggerHandler> of(ref<BuildContext> context);
    StdoutLogger(ref<Widget> child, option<Key> key = nullptr);
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
};
