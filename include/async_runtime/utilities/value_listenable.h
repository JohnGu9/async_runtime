#pragma once
#include "listenable.h"
#include "change_notifier.h"
#include "getter_setter.h"

template <typename T = Object::Void>
class ValueListenable;

template <>
class ValueListenable<Object::Void> : public virtual Listenable
{
protected:
    ValueListenable() {}
};

template <typename T>
class ValueListenable : public ValueListenable<Object::Void>, public getter<T>
{
};
