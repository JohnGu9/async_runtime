#pragma once

#include "thread_pool.h"
#include "../elements/element.h"
#include "../widgets/stateful_widget.h"
#include "../widgets/inherited_widget.h"

class Scheduler : public StatefulWidget
{
public:
    using Handler = Object::Ref<ThreadPool>;
    static Handler of(Object::Ref<BuildContext> context);

    Scheduler(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    Object::Ref<Widget> child;
    Object::Ref<State<StatefulWidget>> createState() override;
};

class SchedulerElement;
class SchedulerProxy : public InheritedWidget
{
    friend Scheduler;
    friend SchedulerElement;
    using Handler = Scheduler::Handler;
    bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) override;
    Object::Ref<Element> createElement() override;

protected:
    Handler _handler;

public:
    SchedulerProxy(Object::Ref<Widget> child, Handler handler);
};

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