#pragma once

#include "function.h"
#include "../object.h"
#include "container.h"

// abstract class
class Listenable : public Object
{
public:
    virtual void addListener(Function<void(ref<Listenable>)> fn);
    virtual void removeListener(Function<void(ref<Listenable>)> fn);
    virtual void dispose();

protected:
    Listenable();

    ref<Set<Function<void(ref<Listenable>)>>> _listeners;
    bool _isDisposed;
};