#include "framework/basic/function.h"

Object::Ref<VoidCallBack> VoidCallBack::fromFunction(std::function<void()> fn)
{
    return Object::create<_VoidCallBack>(fn);
}

void _VoidCallBack::operator()()
{
    this->_fn();
}