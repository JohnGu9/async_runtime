#pragma once

#include "context.h"

class Element;
class Widget;
class InheritedWidget;

class BuildContext : public Context
{
protected:
    ref<Widget> widget;
    lateref<Map<Object::RuntimeType, lateref<InheritedWidget>>> _inheritances;

public:
    BuildContext(ref<Widget> widget) : widget(widget) {}

    template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type * = nullptr>
    option<T> dependOnInheritedWidgetOfExactType();
};
