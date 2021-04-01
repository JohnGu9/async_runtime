#include <typeinfo>

#include "async_runtime/object.h"
#include "async_runtime/basic/string.h"

Object::RuntimeType Object::runtimeType()
{
    return typeid(*this).name();
}

ref<String> Object::toString()
{
    std::stringstream ss;
    this->toStringStream(ss);
    return ss.str();
}

void Object::toStringStream(std::ostream &os)
{
    os << "RuntimeType: " << this->runtimeType()
       << "[" << (size_t)this << "]";
}
