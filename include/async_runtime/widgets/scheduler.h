#pragma once

#include "stateful_widget.h"
#include "inherited_widget.h"
#include "../fundamental/async.h"

class Scheduler : public StatefulWidget
{
public:
    using Handler = ref<ThreadPool>;
    static Handler of(ref<BuildContext> context);

    Scheduler(ref<Widget> child, option<String> name = nullptr, option<Key> key = nullptr);
    finalref<Widget> child;
    finaloption<String> name;
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
