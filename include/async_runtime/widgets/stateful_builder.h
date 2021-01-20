#pragma once

#include "../basic/function.h"
#include "stateful_widget.h"

class StatefulBuilder : public StatefulWidget
{
public:
    StatefulBuilder(Fn<Object::Ref<Widget>(Object::Ref<BuildContext> /*context*/, Fn<void(Fn<void()>)> /*setState*/)> builder, Object::Ref<Key> key = nullptr);

    Fn<Object::Ref<Widget>(Object::Ref<BuildContext>, Fn<void(Fn<void()>)>)> builder;
    Object::Ref<State<StatefulWidget>> createState() override;
};