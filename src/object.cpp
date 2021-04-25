#include <typeinfo>

#include "async_runtime/object.h"
#include "async_runtime/basic/string.h"

Object::RuntimeType Object::runtimeType()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    return typeid(*this).name();
#else
    return typeid(*this).name();
#endif
}

ref<String> Object::toString()
{
    std::stringstream ss;
    this->toStringStream(ss);
    return ss.str();
}

void Object::toStringStream(std::ostream &os)
{
    os << "class<" << this->runtimeType()
       << ">[" << (size_t)this << ']';
}
