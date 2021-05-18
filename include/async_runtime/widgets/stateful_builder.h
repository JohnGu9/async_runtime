#pragma once

#include "stateful_widget.h"

class StatefulBuilder : public StatefulWidget
{
    class _State;
public:
    using SetStateCallback = Function<void(Function<void()>)>;
    StatefulBuilder(
        Function<ref<Widget>(ref<BuildContext> context, SetStateCallback setState)> builder,
        option<Key> key = nullptr);

    Function<ref<Widget>(ref<BuildContext>, SetStateCallback)> builder;
    ref<State<StatefulWidget>> createState() override;
};