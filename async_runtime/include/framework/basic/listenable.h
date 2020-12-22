#pragma once

#include "function.h"

class Listenable : public virtual Object
{
public:
    Listenable();
    virtual void addListener(Object::Ref<VoidCallBack> fn);
    virtual void removeListener(Object::Ref<VoidCallBack> fn);
    virtual void dispose();

protected:
    std::unordered_map<size_t, Object::Ref<VoidCallBack>> _listeners;
    bool _isDisposed;
};