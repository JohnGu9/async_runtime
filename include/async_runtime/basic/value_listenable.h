#pragma once

#include "listenable.h"
#include "change_notifier.h"

template <typename T = nullptr_t>
class ValueListenable;

template <>
class ValueListenable<nullptr_t> : public virtual Listenable
{
protected:
    ValueListenable() {}
};

template <typename T>
class ValueListenable : public ValueListenable<nullptr_t>
{
public:
    // @mustCallSuper
    virtual T &getValue() = 0;
    virtual const T &getValue() const = 0;
};
