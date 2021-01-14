#include <sstream>
#include <typeinfo>

#include "framework/object.h"

Object::RuntimeType Object::runtimeType()
{
    return typeid(*this).name();
}

String Object::toString()
{
    std::stringstream ss;
    this->toStringStream(ss);
    return ss.str();
}

void Object::toStringStream(std::stringstream &ss)
{
    ss << "RuntimeType: " << this->runtimeType()
       << "[" << (size_t)this << "]";
}

Object::~Object() {}
