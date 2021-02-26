#pragma once

#include "context.h"

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
    ref<T> dependOnInheritedWidgetOfExactType();

protected:
    Map<Object::RuntimeType, ref<Inheritance>> _inheritances;
};
