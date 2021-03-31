#pragma once

#include "stateful_widget.h"

template <typename T>
class FutureBuilder : public StatefulWidget
{
public:
    FutureBuilder(ref<Future<T>> future,
                  Function<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)> builder)
        : future(future), builder(builder) {}

    ref<Future<T>> future;
    Function<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)> builder;

    ref<State<>> createState() override;

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
        ref<Future<T>> future = this->getWidget()->future;
        if (!AsyncSnapshot<>::getCompletedFromFuture(future))
        {
            ref<FutureBuilder<T>::_State> self = self();
            future->than([self, future] {
                if (self->getWidget()->future == future)
                    self->setState([] {});
            });
        }
    }

    void didWidgetUpdated(ref<StatefulWidget> oldWidget) override
    {
        ref<FutureBuilder<T>> old = oldWidget->covariant<FutureBuilder<T>>();
        ref<Future<T>> future = this->getWidget()->future;
        if (old->future != future && !AsyncSnapshot<>::getCompletedFromFuture(future))
        {
            ref<FutureBuilder<T>::_State> self = self();
            future->than([self, future] {
                if (self->getWidget()->future == future)
                    self->setState([] {});
            });
        }
        super::didWidgetUpdated(oldWidget);
    }

    ref<Widget> build(ref<BuildContext> context) override
    {
        return this->getWidget()->builder(
            context,
            Object::create<AsyncSnapshot<T>>(this->getWidget()->future));
    }
};

template <typename T>
ref<State<>> FutureBuilder<T>::createState()
{
    return Object::create<FutureBuilder<T>::_State>();
}