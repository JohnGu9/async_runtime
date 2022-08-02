#pragma once
#include "change_notifier.h"
#include "getter_setter.h"
#include "listenable.h"

template <typename T = Object::Void>
class ValueListenable;

template <>
class ValueListenable<Object::Void> : public virtual Listenable
{
protected:
    ValueListenable() = default;
};

template <typename T>
class ValueListenable : public ValueListenable<Object::Void>, public getter<T>
{
};
