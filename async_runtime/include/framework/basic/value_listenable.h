#pragma once

#include "listenable.h"
#include "change_notifier.h"

template <typename T>
class ValueListenable : public virtual Listenable
{
public:
    // @mustCallSuper
    virtual T &getValue() = 0;
    virtual const T &getValue() const = 0;
};
