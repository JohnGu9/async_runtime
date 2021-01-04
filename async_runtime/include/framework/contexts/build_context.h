#pragma once

#include "context.h"

class Element;
class InheritedWidget;

class Inheritance : public virtual Object
{
};

class BuildContext : public virtual Context
{
public:
    template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type * = nullptr>
    Object::Ref<T> dependOnInheritedWidgetOfExactType();

protected:
    Object::Map<Object::RuntimeType, Object::Ref<Inheritance>> _inheritances;
};
