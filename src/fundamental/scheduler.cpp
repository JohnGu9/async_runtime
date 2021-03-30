#include "async_runtime/elements/inherited_element.h"
#include "async_runtime/fundamental/scheduler.h"

class SchedulerElement : public InheritedElement
{
public:
    SchedulerElement(ref<SchedulerProxy> widget) : InheritedElement(widget) {}

    void attach() override
    {
        this->getHandler()->post([this] { this->InheritedElement::attach(); }).get();
    }

    void detach() override
    {
        this->getHandler()->post([this] { this->InheritedElement::detach(); }).get();
    }

    void build() override
    {
        this->getHandler()->post([this] { this->InheritedElement::build(); }).get();
    }

    void notify(ref<Widget> newWidget) override
    {
        this->getHandler()->post([this, newWidget] { this->InheritedElement::notify(newWidget); }).get();
    }

    void update(ref<Widget> newWidget) override
    {
        this->getHandler()->post([this, newWidget] { this->InheritedElement::update(newWidget); }).get();
    }

    void visitDescendant(Function<bool(ref<Element>)> fn) override
    {
        this->getHandler()->post([this, fn] { this->InheritedElement::visitDescendant(fn); }).get();
    }

    void visitAncestor(Function<bool(ref<Element>)> fn) override
    {
        Scheduler::Handler handler = this->getHandler();
        lateref<ThreadPool> parentHandler;
        if (this->getParentHandler().isNotNull(parentHandler) && handler != parentHandler)
        {
            parentHandler->post([this, fn] { this->InheritedElement::visitAncestor(fn); }).get();
        }
        else
        {
            InheritedElement::visitAncestor(fn);
        }
    }

protected:
    virtual Scheduler::Handler getHandler() { return this->_inheritWidget->covariant<SchedulerProxy>()->_handler; }
    virtual option<ThreadPool> getParentHandler()
    {
        lateref<Element> context;
        if (this->parent.isNotNull(context))
            return Scheduler::of(context);
        return nullptr;
    }
};

class _SchedulerState : public State<Scheduler>
{
    using super = State<Scheduler>;
    lateref<ThreadPool> _handler;

    void initState() override
    {
        super::initState();
        this->_handler = Object::create<ThreadPool>(1, this->getWidget()->name);
    }

    void dispose() override
    {
        this->_handler->dispose();
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext>) override
    {
        return Object::create<SchedulerProxy>(this->getWidget()->child, this->_handler);
    }
};

Scheduler::Scheduler(ref<Widget> child, String name, option<Key> key)
    : child(child), name(name), StatefulWidget(key) {}

ref<State<StatefulWidget>> Scheduler::createState()
{
    return Object::create<_SchedulerState>();
}

Scheduler::Handler Scheduler::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<SchedulerProxy>().assertNotNull()->_handler;
}

SchedulerProxy::SchedulerProxy(ref<Widget> child, Handler handler)
    : _handler(handler), InheritedWidget(child, Object::create<ValueKey<Handler>>(handler))
{
    assert(handler->threads() == 1);
}

bool SchedulerProxy::updateShouldNotify(ref<InheritedWidget> oldWidget)
{
    ref<SchedulerProxy> old = oldWidget->covariant<SchedulerProxy>();
    return old->_handler != this->_handler;
}

ref<Element> SchedulerProxy::createElement()
{
    return Object::create<SchedulerElement>(self());
}
