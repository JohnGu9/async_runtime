#pragma once

#include "stateful_widget.h"

template <typename T>
class StreamBuilder : public StatefulWidget
{
    class _State;

public:
    using Builder = Funtion<ref<Widget>(ref<BuildContext>, ref<AsyncSnapshot<T>>)>
    StreamBuilder(ref<Stream<T>> stream, Builder builder, option<Key> key = nullptr)
        : StatefulWidget(key),
          stream(stream), builder(builder) {}

    ref<Stream<T>> stream;
    Builder builder;
    ref<State<StatefulWidget>> createState() override;
};

template <typename T>
class StreamBuilder<T>::_State : public State<StreamBuilder<T>>
{
    ref<Widget> build(ref<BuildContext>) override;
};

template <typename T>
inline ref<State<StatefulWidget>> StreamBuilder<T>::createState()
{
    return Object::create<_State>();
}