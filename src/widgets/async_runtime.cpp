#include "async_runtime.h"
#include "async_runtime/elements/root_element.h"
#include "async_runtime/utilities/console_format.h"

static void task(ref<Widget> widget);

void runApp(ref<Widget> widget)
{
    EventLoop::run([widget]
                   { task(widget); });
}

static void task(ref<Widget> widget)
{
#ifndef NDEBUG
    info_print("Debug mode on");
    info_print("Root widget is " << widget->toString());
#else
    printf("\033c"); // clear console
    info_print("Debug mode off");
#endif

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " start");

    auto handle = EventLoop::Handle::create();
    auto completer = Object::create<Completer<int>>();
    completer->then<int>([handle](const int &) { //
        handle->dispose();
        return 0;
    });

    ref<RootElement> root = Object::create<RootElement>(widget, [completer] { //
        if (!completer->completed())
            completer->complete(0);
    });

    completer->then<int>([root](const int &) { //
        root->detach();
        info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited");
        return 0;
    });
    root->attach();
}

namespace _async_runtime
{
    const char *debug_logger_type = "DEBUG";
    const char *info_logger_type = "INFO ";
    const char *warning_logger_type = "WARNING";
    const char *error_logger_type = "ERROR";
}
