#pragma once

#include "../widgets/inherited_widget.h"
#include "context.h"

class BuildContext : public Context
{
protected:
    ref<Widget> widget;
    lateref<Map<Object::RuntimeType, lateref<InheritedWidget>>> _inheritances;

public:
    BuildContext(ref<Widget> widget) : widget(widget) {}

    template <typename T, typename Object::Base<InheritedWidget>::isBaseOf<T>::type * = nullptr>
    option<T> dependOnInheritedWidgetOfExactType();
};
