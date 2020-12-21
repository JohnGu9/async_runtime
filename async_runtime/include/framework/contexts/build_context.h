#pragma once

#include "context.h"

class Element;

class Inherit : public virtual Object
{
};

class BuildContext : public virtual Context
{
public:
    BuildContext() : _inherits(nullptr) {}

protected:
    Object::Ref<Object::Map<Object::RuntimeType, Object::Ref<Inherit>>> _inherits;
};