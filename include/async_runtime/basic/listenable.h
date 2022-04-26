#pragma once

#include "../object.h"

// abstract class
class Listenable : public virtual Object
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