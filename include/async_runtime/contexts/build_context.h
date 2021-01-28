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

    template <typename T, typename std::enable_if<std::is_base_of<Inheritance, T>::value>::type * = nullptr>
    Object::Ref<T> dependOnInheritanceOfExactType()
    {
        auto iter = this->_inheritances.find(typeid(T).name());
        if (iter == this->_inheritances.end())
            return nullptr;
        return iter->second->cast<T>();
    }

protected:
    Map<Object::RuntimeType, Object::Ref<Inheritance>> _inheritances;
};
