#include "async_runtime.h"

void runApp(Object::Ref<Widget> widget)
{
#ifndef DEBUG
    printf("\033c");
    info_print("Debug mode off");
#endif

    debug_print("Debug mode on");
    debug_print("Root widget is \"" << widget->toString() << "\"");
    auto root = Object::create<RootElement>(widget);
    root->scheduleRootWidget();
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited");
}
