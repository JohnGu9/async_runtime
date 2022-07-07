#pragma once
#include "../object/object.h"
#include <cmath>

// https://en.cppreference.com/w/cpp/language/types

class Number : public virtual Object
{
protected:
    static long long toSigned(const size_t &);

public:
    class Type
    {
    public:
        enum Value
        {
            SizeType,
            Short,
            Integer,
            Long,
            LongLong,
            Float,
            Double,
        };
    };

    virtual Number::Type::Value type() = 0;
    template <typename T>
    class Basic;

    class SizeType;

    class Short;    // at least 16 bits
    class Integer;  // at least 16 bits
    class Long;     // at least 32 bits
    class LongLong; // at least 64 bits

    class Float;  // 32 bits
    class Double; // 64 bits

    using Object::operator==;
    virtual bool operator==(const size_t &other) = 0;
    virtual bool operator==(const short &other) = 0;
    virtual bool operator==(const int &other) = 0;
    virtual bool operator==(const long &other) = 0;
    virtual bool operator==(const long long &other) = 0;
    virtual bool operator==(const float &other) = 0;
    virtual bool operator==(const double &other) = 0;

    virtual bool operator<(const ref<Number> &) = 0;
    virtual bool operator>(const ref<Number> &) = 0;
    virtual bool operator<(const option<Number> &) = 0;
    virtual bool operator>(const option<Number> &) = 0;

    virtual bool operator<(const size_t &other) = 0;
    virtual bool operator<(const short &other) = 0;
    virtual bool operator<(const int &other) = 0;
    virtual bool operator<(const long &other) = 0;
    virtual bool operator<(const long long &other) = 0;
    virtual bool operator<(const float &other) = 0;
    virtual bool operator<(const double &other) = 0;

    virtual bool operator>(const size_t &other) = 0;
    virtual bool operator>(const short &other) = 0;
    virtual bool operator>(const int &other) = 0;
    virtual bool operator>(const long &other) = 0;
    virtual bool operator>(const long long &other) = 0;
    virtual bool operator>(const float &other) = 0;
    virtual bool operator>(const double &other) = 0;
};

template <typename T>
class Number::Basic : public Number
{
public:
    const T value;
    Basic(const T &value) : value(value) {}

    void toStringStream(std::ostream &os) override { os << value; }
    bool operator==(const option<Object> &other) override;

    bool operator==(const size_t &other) override { return this->value == Number::toSigned(other); }
    bool operator==(const short &other) override { return this->value == other; }
    bool operator==(const int &other) override { return this->value == other; }
    bool operator==(const long &other) override { return this->value == other; }
    bool operator==(const long long &other) override { return this->value == other; }
    bool operator==(const float &other) override { return this->value == other; }
    bool operator==(const double &other) override { return this->value == other; }

    bool operator<(const ref<Number> &) override;
    bool operator>(const ref<Number> &) override;
    bool operator<(const option<Number> &number) override;
    bool operator>(const option<Number> &number) override;

    bool operator<(const size_t &other) override { return this->value < Number::toSigned(other); }
    bool operator<(const short &other) override { return this->value < other; }
    bool operator<(const int &other) override { return this->value < other; }
    bool operator<(const long &other) override { return this->value < other; }
    bool operator<(const long long &other) override { return this->value < other; }
    bool operator<(const float &other) override { return this->value < other; }
    bool operator<(const double &other) override { return this->value < other; }

    bool operator>(const size_t &other) override { return this->value > Number::toSigned(other); }
    bool operator>(const short &other) override { return this->value > other; }
    bool operator>(const int &other) override { return this->value > other; }
    bool operator>(const long &other) override { return this->value > other; }
    bool operator>(const long long &other) override { return this->value > other; }
    bool operator>(const float &other) override { return this->value > other; }
    bool operator>(const double &other) override { return this->value > other; }
};

class Number::SizeType : public Number
{
public:
    const size_t value;
    SizeType(const size_t &value) : value(value) {}

    Number::Type::Value type() override; // Number::Type::SizeType
    size_t hashCode() override;

    void toStringStream(std::ostream &os) override;
    bool operator==(const option<Object> &other) override;

    bool operator==(const size_t &other) override;
    bool operator==(const short &other) override;
    bool operator==(const int &other) override;
    bool operator==(const long &other) override;
    bool operator==(const long long &other) override;
    bool operator==(const float &other) override;
    bool operator==(const double &other) override;

    bool operator<(const ref<Number> &) override;
    bool operator>(const ref<Number> &) override;

    bool operator<(const option<Number> &number) override;
    bool operator>(const option<Number> &number) override;

    bool operator<(const size_t &other) override;
    bool operator<(const short &other) override;
    bool operator<(const int &other) override;
    bool operator<(const long &other) override;
    bool operator<(const long long &other) override;
    bool operator<(const float &other) override;
    bool operator<(const double &other) override;

    bool operator>(const size_t &other) override;
    bool operator>(const short &other) override;
    bool operator>(const int &other) override;
    bool operator>(const long &other) override;
    bool operator>(const long long &other) override;
    bool operator>(const float &other) override;
    bool operator>(const double &other) override;
};

class Number::Short : public Number::Basic<short>
{
public:
    using Number::Basic<short>::Basic;
    Number::Type::Value type() override; // Number::Type::Short
    size_t hashCode() override;
};

class Number::Integer : public Number::Basic<int>
{
public:
    using Number::Basic<int>::Basic;
    Number::Type::Value type() override; // Number::Type::Integer
    size_t hashCode() override;
};

class Number::Long : public Number::Basic<long>
{
public:
    using Number::Basic<long>::Basic;
    Number::Type::Value type() override; // Number::Type::Long
    size_t hashCode() override;
};

class Number::LongLong : public Number::Basic<long long>
{
public:
    using Number::Basic<long long>::Basic;
    Number::Type::Value type() override; // Number::Type::LongLong
    size_t hashCode() override;
};

class Number::Float : public Number::Basic<float>
{
public:
    using Number::Basic<float>::Basic;
    Number::Type::Value type() override; // Number::Type::Float
    size_t hashCode() override;
};

class Number::Double : public Number::Basic<double>
{
public:
    using Number::Basic<double>::Basic;
    Number::Type::Value type() override; // Number::Type::Double
    size_t hashCode() override;
};

template <>
class ref<Number> : public _async_runtime::RefImplement<Number>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Number>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Number, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Number, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    ref(const size_t &value) : super(Object::create<Number::SizeType>(value)) {}

    ref(const short &value) : super(Object::create<Number::Short>(value)) {}
    ref(const int &value) : super(Object::create<Number::Integer>(value)) {}
    ref(const long &value) : super(Object::create<Number::Long>(value)) {}
    ref(const long long &value) : super(Object::create<Number::LongLong>(value)) {}

    ref(const float &value) : super(Object::create<Number::Float>(value)) {}
    ref(const double &value) : super(Object::create<Number::Double>(value)) {}

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Number>::RefImplement;
};

template <typename T>
bool Number::Basic<T>::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<Number *>(other.get()))
    {
        switch (ptr->type())
        {
        case Number::Type::SizeType:
            return this->value == Number::toSigned(dynamic_cast<Number::SizeType *>(ptr)->value);
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

template <typename T>
bool Number::Basic<T>::operator<(const ref<Number> &other)
{
    switch (other->type())
    {
    case Number::Type::SizeType:
        return this->value < Number::toSigned(dynamic_cast<Number::SizeType *>(other.get())->value);
    case Number::Type::Short:
        return this->value < dynamic_cast<Number::Short *>(other.get())->value;
    case Number::Type::Integer:
        return this->value < dynamic_cast<Number::Integer *>(other.get())->value;
    case Number::Type::Long:
        return this->value < dynamic_cast<Number::Long *>(other.get())->value;
    case Number::Type::LongLong:
        return this->value < dynamic_cast<Number::LongLong *>(other.get())->value;
    case Number::Type::Float:
        return this->value < dynamic_cast<Number::Float *>(other.get())->value;
    case Number::Type::Double:
        return this->value < dynamic_cast<Number::Double *>(other.get())->value;
    default:
        throw NotImplementedError();
    }
}

template <typename T>
bool Number::Basic<T>::operator>(const ref<Number> &other)
{
    switch (other->type())
    {
    case Number::Type::SizeType:
        return this->value > Number::toSigned(dynamic_cast<Number::SizeType *>(other.get())->value);
    case Number::Type::Short:
        return this->value > dynamic_cast<Number::Short *>(other.get())->value;
    case Number::Type::Integer:
        return this->value > dynamic_cast<Number::Integer *>(other.get())->value;
    case Number::Type::Long:
        return this->value > dynamic_cast<Number::Long *>(other.get())->value;
    case Number::Type::LongLong:
        return this->value > dynamic_cast<Number::LongLong *>(other.get())->value;
    case Number::Type::Float:
        return this->value > dynamic_cast<Number::Float *>(other.get())->value;
    case Number::Type::Double:
        return this->value > dynamic_cast<Number::Double *>(other.get())->value;
    default:
        throw NotImplementedError();
    }
}

template <typename T>
bool Number::Basic<T>::operator<(const option<Number> &number)
{
    if_not_null(number) return this->operator<(number);
    end_if() return false;
}

template <typename T>
bool Number::Basic<T>::operator>(const option<Number> &number)
{
    if_not_null(number) return this->operator>(number);
    end_if() return true;
}
