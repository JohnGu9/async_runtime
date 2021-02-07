#pragma once

#include "stateful_widget.h"

template <typename T>
class FutureBuilder : public StatefulWidget
{
public:
    FutureBuilder(Object::Ref<Future<T>> future,
                  Function<Object::Ref<Widget>(Object::Ref<BuildContext>, Object::Ref<AsyncSnapshot<T>>)> builder)
        : future(future), builder(builder) { assert(future && builder); }

    Object::Ref<Future<T>> future;
    Function<Object::Ref<Widget>(Object::Ref<BuildContext>, Object::Ref<AsyncSnapshot<T>>)> builder;

    Object::Ref<State<>> createState() override;

protected:
    class _State;
};

template <typename T>
class FutureBuilder<T>::_State : public State<FutureBuilder<T>>
{
public:
    using super = State<FutureBuilder<T>>;

    void initState() override
    {
        super::initState();
        Object::Ref<Future<T>> future = this->getWidget()->future;
        if (not AsyncSnapshot<>::getCompletedFromFuture(future))
        {
            Object::Ref<FutureBuilder<T>::_State> self = Object::cast<>(this);
            future->than([self, future] {
                if (self->getWidget()->future == future)
                    self->setState([] {});
            });
        }
    }

    void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) override
    {
        Object::Ref<FutureBuilder<T>> old = oldWidget->covariant<FutureBuilder<T>>();
        Object::Ref<Future<T>> future = this->getWidget()->future;
        if (old->future != future && (not AsyncSnapshot<>::getCompletedFromFuture(future)))
        {
            Object::Ref<FutureBuilder<T>::_State> self = Object::cast<>(this);
            future->than([self, future] {
                if (self->getWidget()->future == future)
                    self->setState([] {});
            });
        }
        super::didWidgetUpdated(oldWidget);
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return this->getWidget()->builder(
            context,
            Object::create<AsyncSnapshot<T>>(this->getWidget()->future));
    }
};

template <typename T>
Object::Ref<State<>> FutureBuilder<T>::createState()
{
    return Object::create<FutureBuilder<T>::_State>();
}