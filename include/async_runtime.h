#pragma once

// export library
#include "async_runtime/widgets/widget.h"
#include "async_runtime/widgets/leaf_widget.h"
#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/widgets/multi_child_widget.h"
#include "async_runtime/widgets/named_multi_child_widget.h"
#include "async_runtime/widgets/inherited_widget.h"
#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/widgets/builder.h"
#include "async_runtime/widgets/value_listenable_builder.h"
#include "async_runtime/widgets/process.h"
#include "async_runtime/widgets/future_builder.h"

#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/fundamental/http.h"

#include "async_runtime/elements/root_element.h"
inline int runApp(ref<Widget> widget)
{
#ifndef NDEBUG
    info_print("Debug mode on");
    info_print("Root widget is " << widget->toString());
#else
    printf("\033c");
    info_print("Debug mode off");
#endif

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " start");

    ref<RootElement> root = Object::create<RootElement>(widget);
    root->attach();
#ifndef ASYNC_RUNTIME_DISABLE_CONSOLE
    root->_console();
#else
    root->_noConsole();
#endif
    root->detach();
    const auto &exitCode = root->exitCode;

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited with code " << exitCode);
    return exitCode;
}
