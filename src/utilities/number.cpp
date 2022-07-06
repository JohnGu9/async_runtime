#include "async_runtime/utilities/number.h"

static auto integer_hash = std::hash<long long>();
static auto double_hash = std::hash<double>();

// SizeType
long long Number::SizeType::toSigned() { return static_cast<long long>(this->value); }
Number::Type::Value Number::SizeType::type() { return Number::Type::SizeType; }
size_t Number::SizeType::hashCode() { return integer_hash(toSigned()); }
bool Number::SizeType::operator==(const size_t &other) { return this->value == other; }
bool Number::SizeType::operator==(const short &other) { return toSigned() == other; }
bool Number::SizeType::operator==(const int &other) { return toSigned() == other; }
bool Number::SizeType::operator==(const long &other) { return toSigned() == other; }
bool Number::SizeType::operator==(const long long &other) { return toSigned() == other; }
bool Number::SizeType::operator==(const float &other) { return toSigned() == other; }
bool Number::SizeType::operator==(const double &other) { return toSigned() == other; }
bool Number::SizeType::operator<(const size_t &other) { return this->value < other; }
bool Number::SizeType::operator<(const short &other) { return toSigned() < other; }
bool Number::SizeType::operator<(const int &other) { return toSigned() < other; }
bool Number::SizeType::operator<(const long &other) { return toSigned() < other; }
bool Number::SizeType::operator<(const long long &other) { return toSigned() < other; }
bool Number::SizeType::operator<(const float &other) { return toSigned() < other; }
bool Number::SizeType::operator<(const double &other) { return toSigned() < other; }
bool Number::SizeType::operator>(const size_t &other) { return this->value < other; }
bool Number::SizeType::operator>(const short &other) { return toSigned() > other; }
bool Number::SizeType::operator>(const int &other) { return toSigned() > other; }
bool Number::SizeType::operator>(const long &other) { return toSigned() > other; }
bool Number::SizeType::operator>(const long long &other) { return toSigned() > other; }
bool Number::SizeType::operator>(const float &other) { return toSigned() > other; }
bool Number::SizeType::operator>(const double &other) { return toSigned() > other; }
bool Number::SizeType::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<Number *>(other.get()))
    {
        switch (ptr->type())
        {
        case Number::Type::SizeType:
            return this->value == dynamic_cast<Number::SizeType *>(ptr)->value;
        case Number::Type::Short:
            return this->value == dynamic_cast<Number::Short *>(ptr)->value;
        case Number::Type::Integer:
            return this->value == dynamic_cast<Number::Integer *>(ptr)->value;
        case Number::Type::Long:
            return this->value == dynamic_cast<Number::Long *>(ptr)->value;
        case Number::Type::LongLong:
            return this->value == dynamic_cast<Number::LongLong *>(ptr)->value;
        case Number::Type::Float:
            return this->value == dynamic_cast<Number::Float *>(ptr)->value;
        case Number::Type::Double:
            return this->value == dynamic_cast<Number::Double *>(ptr)->value;
        default:
            break;
        }
    }
    return false;
}
bool Number::SizeType::operator<(const ref<Number> &other)
{
    switch (other->type())
    {
    case Number::Type::SizeType:
        return this->value < dynamic_cast<Number::SizeType *>(other.get())->value;
    case Number::Type::Short:
        return toSigned() < dynamic_cast<Number::Short *>(other.get())->value;
    case Number::Type::Integer:
        return toSigned() < dynamic_cast<Number::Integer *>(other.get())->value;
    case Number::Type::Long:
        return toSigned() < dynamic_cast<Number::Long *>(other.get())->value;
    case Number::Type::LongLong:
        return toSigned() < dynamic_cast<Number::LongLong *>(other.get())->value;
    case Number::Type::Float:
        return toSigned() < dynamic_cast<Number::Float *>(other.get())->value;
    case Number::Type::Double:
        return toSigned() < dynamic_cast<Number::Double *>(other.get())->value;
    default:
        throw NotImplementedError();
    }
}
bool Number::SizeType::operator>(const ref<Number> &other)
{
    switch (other->type())
    {
    case Number::Type::SizeType:
        return this->value > dynamic_cast<Number::SizeType *>(other.get())->value;
    case Number::Type::Short:
        return toSigned() > dynamic_cast<Number::Short *>(other.get())->value;
    case Number::Type::Integer:
        return toSigned() > dynamic_cast<Number::Integer *>(other.get())->value;
    case Number::Type::Long:
        return toSigned() > dynamic_cast<Number::Long *>(other.get())->value;
    case Number::Type::LongLong:
        return toSigned() > dynamic_cast<Number::LongLong *>(other.get())->value;
    case Number::Type::Float:
        return toSigned() > dynamic_cast<Number::Float *>(other.get())->value;
    case Number::Type::Double:
        return toSigned() > dynamic_cast<Number::Double *>(other.get())->value;
    default:
        throw NotImplementedError();
    }
}
bool Number::SizeType::operator<(const option<Number> &number)
{
    if_not_null(number) return this->operator<(number);
    end_if() return false;
}
bool Number::SizeType::operator>(const option<Number> &number)
{
    if_not_null(number) return this->operator>(number);
    end_if() return true;
}

// Short
Number::Type::Value Number::Short::type() { return Number::Type::Short; }
size_t Number::Short::hashCode() { return integer_hash(value); }

// Integer
Number::Type::Value Number::Integer::type() { return Number::Type::Integer; }
size_t Number::Integer::hashCode() { return integer_hash(value); }

// Long
Number::Type::Value Number::Long::type() { return Number::Type::Long; }
size_t Number::Long::hashCode() { return integer_hash(value); }

// LongLong
Number::Type::Value Number::LongLong::type() { return Number::Type::LongLong; }
size_t Number::LongLong::hashCode() { return integer_hash(value); }

// Float
Number::Type::Value Number::Float::type() { return Number::Type::Float; }
size_t Number::Float::hashCode() { return double_hash(value); }

// Double
Number::Type::Value Number::Double::type() { return Number::Type::Double; }
size_t Number::Double::hashCode() { return double_hash(value); }
