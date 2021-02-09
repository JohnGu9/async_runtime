#pragma once

#include "../widgets/stateful_widget.h"
#include "../widgets/inherited_widget.h"
#include "async.h"

class Scheduler : public StatefulWidget
{
public:
    using Handler = Object::Ref<ThreadPool>;
    static Handler of(Object::Ref<BuildContext> context);

    Scheduler(Object::Ref<Widget> child, String name = nullptr, Object::Ref<Key> key = nullptr);
    Object::Ref<Widget> child;
    String name;
    Object::Ref<State<StatefulWidget>> createState() override;
};

class SchedulerProxy : public InheritedWidget
{
    friend class Scheduler;
    friend class SchedulerElement;
    using Handler = Scheduler::Handler;
    bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) override;
    Object::Ref<Element> createElement() override;

protected:
    Handler _handler;

public:
    SchedulerProxy(Object::Ref<Widget> child, Handler handler);
};
