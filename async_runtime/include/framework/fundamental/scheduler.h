#pragma once

#include "thread_pool.h"
#include "../widgets/widget.h"
#include "../elements/element.h"

class Scheduler : public virtual StatelessWidget
{
public:
    using Handler = Object::Ref<ThreadPool>;
    static Handler of(Object::Ref<BuildContext> context);
    Scheduler(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    Object::Ref<Widget> _child;

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    Object::Ref<Element> createElement() override;
};

class SchedulerElement : public virtual StatelessElement, public virtual Inheritance
{
public:
    SchedulerElement(Object::Ref<Scheduler> widget);
    Scheduler::Handler _handler;
    void attach() override;
    void detach() override;
    void build() override;
    void notify(Object::Ref<Widget> newWidget) override;
    void update(Object::Ref<Widget> newWidget) override;

protected:
    virtual void toBuild();
    virtual void toNotify(Object::Ref<Widget> newWidget);
    virtual void toUpdate(Object::Ref<Widget> newWidget);
};