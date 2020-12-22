#pragma once

#include "context.h"

class Element;

class Inherit : public virtual Object
{
};

class BuildContext : public virtual Context
{
public:

protected:
    Object::Map<Object::RuntimeType, Object::Ref<Inherit>> _inherits;
};