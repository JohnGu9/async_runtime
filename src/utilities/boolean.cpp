#include "async_runtime/utilities/boolean.h"

bool Boolean::operator==(const bool &other) const
{
    return this->value == other;
}

bool Boolean::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<Boolean *>(other.get()))
        return this->value == ptr->value;
    return false;
}

size_t Boolean::hashCode()
{
    return std::hash<bool>()(value);
}

void Boolean::toStringStream(std::ostream &os)
{
    if (value)
        os << "true";
    else
        os << "false";
}
