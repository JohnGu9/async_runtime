#pragma once

#include "../basic/function.h"
#include "stateful_widget.h"

class StatefulBuilder : public StatefulWidget
{
public:
    StatefulBuilder(
        Function<Object::Ref<Widget>(Object::Ref<BuildContext> context, Function<void(Function<void()>)> setState)> builder,
        Object::Ref<Key> key = nullptr);

    Function<Object::Ref<Widget>(Object::Ref<BuildContext>, Function<void(Function<void()>)>)> builder;
    Object::Ref<State<StatefulWidget>> createState() override;
};