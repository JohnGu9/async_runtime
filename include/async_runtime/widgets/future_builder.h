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
        ref<Future<T>> future = this->widget->future;
        if (!AsyncSnapshot<>::getCompletedFromFuture(future))
        {
            ref<FutureBuilder<T>::_State> self = self();
            future->than([self, future] {
                if (self->widget->future == future)
                    self->setState([] {});
            });
        }
    }

    void didWidgetUpdated(ref<FutureBuilder<T>> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        ref<Future<T>> future = this->widget->future;
        if (oldWidget->future != future && !AsyncSnapshot<>::getCompletedFromFuture(future))
        {
            ref<FutureBuilder<T>::_State> self = self();
            future->than([self, future] {
                if (self->widget->future == future)
                    self->setState([] {});
            });
        }
    }

    ref<Widget> build(ref<BuildContext> context) override
    {
        return this->widget->builder(
            context,
            Object::create<AsyncSnapshot<T>>(this->widget->future));
    }
};

template <typename T>
ref<State<>> FutureBuilder<T>::createState()
{
    return Object::create<FutureBuilder<T>::_State>();
}