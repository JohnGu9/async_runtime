#include "async_runtime/fundamental/scheduler.h"

class SchedulerElement : public InheritedElement
{
public:
    SchedulerElement(Object::Ref<SchedulerProxy> widget) : InheritedElement(widget) {}

    void attach() override
    {
        this->getHandler()->post([this] { this->toAttach(); }).get();
    }

    void detach() override
    {
        this->getHandler()->post([this] { this->toDetach(); }).get();
    }

    void build() override
    {
        this->getHandler()->post([this] { this->toBuild(); }).get();
    }

    void notify(Object::Ref<Widget> newWidget) override
    {
        this->getHandler()->post([this, newWidget] { this->toNotify(newWidget); }).get();
    }

    void update(Object::Ref<Widget> newWidget) override
    {
        this->getHandler()->post([this, newWidget] { this->toUpdate(newWidget); }).get();
    }

    void visitDescendant(Function<bool(Object::Ref<Element>)> fn) override
    {
        this->getHandler()->post([this, fn] { this->toVisitDescendant(fn); }).get();
    }

    void visitAncestor(Function<bool(Object::Ref<Element>)> fn) override
    {
        Scheduler::Handler handler = this->getHandler();
        Scheduler::Handler parentHandler = this->getParentHandler();
        if (handler != parentHandler && parentHandler != nullptr)
            parentHandler->post([this, fn] { this->toVisitAncestor(fn); }).get();
        else
            InheritedElement::visitAncestor(fn);
    }

protected:
    virtual Scheduler::Handler getHandler() { return this->_inheritWidget->cast<SchedulerProxy>()->_handler; }
    virtual Scheduler::Handler getParentHandler() { return Scheduler::of(this->parent.lock()); }
    virtual void toAttach() { InheritedElement::attach(); }
    virtual void toDetach() { InheritedElement::detach(); }
    virtual void toBuild() { InheritedElement::build(); }
    virtual void toNotify(Object::Ref<Widget> newWidget) { InheritedElement::notify(newWidget); }
    virtual void toUpdate(Object::Ref<Widget> newWidget) { InheritedElement::update(newWidget); }
    virtual void toVisitDescendant(Function<bool(Object::Ref<Element>)> fn) { InheritedElement::visitDescendant(fn); }
    virtual void toVisitAncestor(Function<bool(Object::Ref<Element>)> fn) { InheritedElement::visitAncestor(fn); }
};

class _SchedulerState : public State<Scheduler>
{
    using super = State<Scheduler>;
    Scheduler::Handler _handler;

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

    Object::Ref<Widget> build(Object::Ref<BuildContext>) override
    {
        return Object::create<SchedulerProxy>(this->getWidget()->child, this->_handler);
    }
};

Scheduler::Scheduler(Object::Ref<Widget> child, String name, Object::Ref<Key> key)
    : child(child), name(name), StatefulWidget(key) { assert(this->child); }

Object::Ref<State<StatefulWidget>> Scheduler::createState()
{
    return Object::create<_SchedulerState>();
}

Scheduler::Handler Scheduler::of(Object::Ref<BuildContext> context)
{
    if (Object::Ref<SchedulerProxy> proxy = context->dependOnInheritedWidgetOfExactType<SchedulerProxy>())
        return proxy->_handler;
    return nullptr;
}

SchedulerProxy::SchedulerProxy(Object::Ref<Widget> child, Handler handler)
    : _handler(handler), InheritedWidget(child, Object::create<ValueKey<Handler>>(handler))
{
    assert(handler->threads() == 1);
}

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
    return Object::create<SchedulerElement>(Object::cast<>(this));
}
