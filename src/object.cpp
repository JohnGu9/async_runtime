#include <typeinfo>

#include "async_runtime/object.h"

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
#if defined(_WIN32)
       << &(typeid(*this).name()[6]) // from "class MyClass" to "MyClass"
#else
       << typeid(*this).name()
#endif
       << ">[" << (size_t)this << ']';
}
