#pragma once

#include "function.h"

class Listenable : public virtual Object
{
public:
    Listenable();
    virtual void addListener(Object::Ref<Fn<void(Object::Ref<Listenable>)>> fn);
    virtual void removeListener(Object::Ref<Fn<void(Object::Ref<Listenable>)>> fn);
    virtual void dispose();

protected:
    std::unordered_map<size_t, Object::Ref<Fn<void(Object::Ref<Listenable>)>>> _listeners;
    bool _isDisposed;
};