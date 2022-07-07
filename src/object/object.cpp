#include "async_runtime/object/object.h"
#include <typeinfo>

bool Object::operator==(const option<Object> &other)
{
    return Object::identical<>(self(), other);
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
    os << typeid(*this).name() << "[" << (size_t)this << ']';
}

static const auto hs = std::hash<std::shared_ptr<Object>>();
size_t Object::hashCode()
{
    return hs(static_cast<std::enable_shared_from_this<Object> *>(this)->shared_from_this());
}

bool Object::Void::operator==(const option<Object> &other)
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

std::recursive_mutex Object::OstreamCallStack::mutex;

size_t Object::OstreamCallStack::depth(0);
