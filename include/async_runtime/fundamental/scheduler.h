#pragma once

#include "../widgets/stateful_widget.h"
#include "../widgets/inherited_widget.h"
#include "async.h"

class Scheduler : public StatefulWidget
{
public:
    using Handler = ref<ThreadPool>;
    static Handler of(ref<BuildContext> context);

    Scheduler(ref<Widget> child, String name = nullptr, option<Key> key = nullptr);
    ref<Widget> child;
    String name;
    ref<State<StatefulWidget>> createState() override;
};

class SchedulerProxy : public InheritedWidget
{
    friend class Scheduler;
    friend class SchedulerElement;
    using Handler = Scheduler::Handler;
    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override;
    ref<Element> createElement() override;

protected:
    Handler _handler;

public:
    SchedulerProxy(ref<Widget> child, Handler handler);
};
