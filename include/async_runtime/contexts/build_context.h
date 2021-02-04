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
    BuildContext(Object::Ref<Widget> widget);
    Object::Ref<Widget> widget;

    template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type * = nullptr>
    Object::Ref<T> dependOnInheritedWidgetOfExactType();

protected:
    Map<Object::RuntimeType, Object::Ref<Inheritance>> _inheritances;
};
