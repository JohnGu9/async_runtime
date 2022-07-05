#include "async_runtime/object/object.h"
#include <typeinfo>

bool Object::operator==(const option<Object>& other)
{
    return static_cast<std::enable_shared_from_this<Object> *>(this)->shared_from_this() ==
           static_cast<const std::shared_ptr<Object> &>(other);
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

static const auto hs = std::hash<std::shared_ptr<Object>>();
size_t Object::hashCode()
{
    return hs(static_cast<std::enable_shared_from_this<Object> *>(this)->shared_from_this());
}

bool Object::Void::operator==(const option<Object>& other)
{
    auto p = other.get();
    if (p == nullptr)
        return true;
    else if (auto ptr = dynamic_cast<Object::Void *>(p))
        return true;
    return false;
}

void Object::Void::toStringStream(std::ostream &os) { os << "Object::Void"; }

size_t Object::Void::hashCode() { return 0; }
