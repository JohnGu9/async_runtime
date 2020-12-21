#pragma once

#include <functional>
#include "../object.h"

class Function : public virtual Object
{
};

class VoidCallBack : public Function
{
public:
    static Object::Ref<VoidCallBack> fromFunction(std::function<void()> fn);
    virtual void operator()() = 0;
};

class _VoidCallBack : public VoidCallBack
{
public:
    _VoidCallBack(std::function<void()> &fn) : _fn(fn) {}
    virtual void operator()() override;

protected:
    std::function<void()> _fn;
};