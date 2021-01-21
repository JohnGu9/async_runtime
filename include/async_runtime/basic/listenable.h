#pragma once

#include "function.h"

class Listenable : public virtual Object
{
public:
    Listenable();
    virtual void addListener(Function<void(Object::Ref<Listenable>)> fn);
    virtual void removeListener(Function<void(Object::Ref<Listenable>)> fn);
    virtual void dispose();

protected:
    Object::Map<size_t, Function<void(Object::Ref<Listenable>)>> _listeners;
    bool _isDisposed;
};