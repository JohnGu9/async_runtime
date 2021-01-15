#include "framework/fundamental/scheduler.h"

class _SchedulerState : public State<Scheduler>
{
    using super = State<Scheduler>;
    Scheduler::Handler _handler;

    void initState() override
    {
        super::initState();
        this->_handler = Object::create<ThreadPool>(1);
    }

    void dispose() override
    {
        this->_handler->dispose();
        super::dispose();
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext>) override
    {
        return Object::create<SchedulerProxy>(this->getWidget()->_child, this->_handler);
    }
};

Scheduler::Scheduler(Object::Ref<Widget> child, Object::Ref<Key> key) : _child(child), StatefulWidget(key) { assert(this->_child); }

Object::Ref<StatefulWidget::State> Scheduler::createState()
{
    return Object::create<_SchedulerState>();
}

Scheduler::Handler Scheduler::of(Object::Ref<BuildContext> context)
{
    return context->dependOnInheritanceOfExactType<SchedulerProxy>()->_handler;
}

SchedulerProxy::SchedulerProxy(Object::Ref<Widget> child, Handler handler)
    : _handler(handler), InheritedWidget(child, Object::create<ValueKey<Handler>>(handler)) {}

bool SchedulerProxy::updateShouldNotify(Object::Ref<InheritedWidget> oldWidget)
{
    if (Object::Ref<SchedulerProxy> old = oldWidget->cast<SchedulerProxy>())
        return old->_handler != this->_handler;
    else
    {
        assert(false);
        return true;
    }
}

Object::Ref<Element> SchedulerProxy::createElement()
{
    return Object::create<SchedulerElement>(Object::self(this));
}

