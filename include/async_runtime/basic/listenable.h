#pragma once

#include "function.h"
#include "../object.h"
#include "container.h"

// abstract class
class Listenable : public Object
{
public:
    virtual void addListener(Function<void()> fn);
    virtual void removeListener(Function<void()> fn);
    virtual void dispose();

protected:
    Listenable();

    ref<Set<Function<void()>>> _listeners;
    bool _isDisposed;
};