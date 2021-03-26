#pragma once

#include "../basic/function.h"
#include "stateful_widget.h"

class StatefulBuilder : public StatefulWidget
{
public:
    StatefulBuilder(
        Function<ref<Widget>(ref<BuildContext> context, Function<void(Function<void()>)> setState)> builder,
        option<Key> key = nullptr);

    Function<ref<Widget>(ref<BuildContext>, Function<void(Function<void()>)>)> builder;
    ref<State<StatefulWidget>> createState() override;
};