#pragma once

#include "function.h"
#include "../object.h"

// abstract class
class Listenable : public Object
{
public:
    virtual void addListener(Function<void(Object::Ref<Listenable>)> fn);
    virtual void removeListener(Function<void(Object::Ref<Listenable>)> fn);
    virtual void dispose();

protected:
    Listenable();

    Set<Function<void(Object::Ref<Listenable>)>> _listeners;
    bool _isDisposed;
};