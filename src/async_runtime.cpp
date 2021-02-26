#include "async_runtime.h"
#include "async_runtime/elements/root_element.h"

void runApp(ref<Widget> widget)
{
#ifndef DEBUG
    printf("\033c");
    info_print("Debug mode off");
#endif

    debug_print("Debug mode on");
    debug_print("Root widget is [" << widget->toString() << "]");
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " start");

    ref<RootElement> root = Object::create<RootElement>(widget);
    root->scheduleRootWidget();

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited");
}
