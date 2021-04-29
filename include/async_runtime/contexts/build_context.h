#pragma once

#include "context.h"
#include "../basic/container.h"

class Element;
class Widget;
class InheritedWidget;

class Inheritance : public virtual Object
{
};

class BuildContext : public Context
{
public:
    BuildContext(ref<Widget> widget);
    ref<Widget> widget;

    template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type * = nullptr>
    option<T> dependOnInheritedWidgetOfExactType();

protected:
    lateref<Map<Object::RuntimeType, lateref<Inheritance>>> _inheritances;
};
