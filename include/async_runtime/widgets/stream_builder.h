#pragma once

#include "stateful_widget.h"

template <typename T>
class StreamBuilder : public StatefulWidget
{
    class _State;

public:
    using Builder = Function<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)>;
    StreamBuilder(ref<Stream<T>> stream, Builder builder, option<Key> key = nullptr)
        : StatefulWidget(key),
          stream(stream), builder(builder) {}

    finalref<Stream<T>> stream;
    finalref<Fn<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)>> builder;
    ref<State<StatefulWidget>> createState() override;
};

template <typename T>
class StreamBuilder<T>::_State : public State<StreamBuilder<T>>
{
    using super = State<StreamBuilder<T>>;
    lateref<StreamSubscription<T>> _subscription;
    void initState() override {}
    void dispose() override { _subscription->cancel(); }
    ref<Widget> build(ref<BuildContext>) override;
};

template <typename T>
inline ref<State<StatefulWidget>> StreamBuilder<T>::createState()
{
    return Object::create<_State>();
}
