#pragma once

#include "listenable.h"
#include "change_notifier.h"
#include "getter_setter.h"

template <typename T = std::nullptr_t>
class ValueListenable;

template <>
class ValueListenable<std::nullptr_t> : public virtual Listenable
{
protected:
    ValueListenable() {}
};

template <typename T>
class ValueListenable : public ValueListenable<std::nullptr_t>, public getter<T>
{
};
