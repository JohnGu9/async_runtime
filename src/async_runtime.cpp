#include "async_runtime/elements/root_element.h"
#include "async_runtime/widgets/widget.h"

int runApp(ref<Widget> widget, bool withConsole)
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

    if (withConsole)
        root->_console();
    else
        root->_noConsole();

    root->detach();
    const auto &exitCode = root->exitCode;

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited with code " << exitCode);
    return exitCode;
}
