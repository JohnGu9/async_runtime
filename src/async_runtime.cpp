#include "async_runtime/elements/root_element.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/widgets/widget.h"

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
    printf("\033c");
    info_print("Debug mode off");
#endif

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " start");

    auto handle = EventLoop::Handle::create();
    auto completer = Object::create<Completer<int>>();
    completer->then<int>([handle](const int &)
                         { handle->dispose(); return 0; });

    ref<RootElement> root = Object::create<RootElement>(widget, [completer]
                                                        {
        if(!completer->completed())completer->resolve(0); });

    completer->then<int>([root](const int &)
                         {
                           root->detach();
                           info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited");
                           return 0 ; });
    root->attach();
}
