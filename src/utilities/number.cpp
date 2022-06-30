#include "async_runtime/utilities/number.h"

int Number::Integer::toInt() { return value; }

double Number::Integer::toDouble() { return value; }

bool Number::Integer::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<Integer *>(other.get()))
    {
        return this->value == ptr->value;
    }
    return false;
}

size_t Number::Integer::hashCode()
{
    return std::hash<int>()(value);
}

void Number::Integer::toStringStream(std::ostream &os) { os << value; }

int Number::Double::toInt() { return static_cast<int>(value); }

double Number::Double::toDouble() { return value; }

bool Number::Double::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<Double *>(other.get()))
    {
        return this->value == ptr->value;
    }
    return false;
}

size_t Number::Double::hashCode()
{
    return std::hash<double>()(value);
}

void Number::Double::toStringStream(std::ostream &os) { os << value; }
