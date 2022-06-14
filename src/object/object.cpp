#include "async_runtime/object/object.h"
#include <typeinfo>

bool Object::operator==(ref<Object> other)
{
    return false; // if this function be called, the [other] must be the 'another' Object
}

Object::RuntimeType Object::runtimeType()
{
    return typeid(*this).hash_code();
}

ref<String> Object::toString()
{
    std::stringstream ss;
    this->toStringStream(ss);
    return ss.str();
}

void Object::toStringStream(std::ostream &os)
{
    os << "class<"
#ifdef _WIN32
       << &(typeid(*this).name()[6]) // from "class MyClass" to "MyClass"
#else
       << typeid(*this).name()
#endif
       << ">[" << (size_t)this << ']';
}

namespace _async_runtime
{
    size_t ostreamStackDepth = 0;
};