#pragma once

#include "../../object/object.h"
#include <type_traits>

class Number : public virtual Object
{
protected:
    /**
     * @brief cmp signed int with size_t
     *
     * @tparam T
     * @return int
     * less than 0: less
     * equal 0: equal
     * more than 0: more
     */
    template <typename T>
    static int cmp(const size_t &, const T &);
    template <typename T>
    static int cmp(const T &, const size_t &);

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
            LongDouble,
        };
    };

    virtual Number::Type::Value type() = 0;
    template <typename T>
    class Basic;

    template <typename T>
    class SignedBasic;

    template <typename T>
    class IntegerBasic;
    template <typename T>
    class DoubleBasic;

    class SizeType; // system size_t

    class Short;    // at least 16 bits
    class Integer;  // at least 16 bits
    class Long;     // at least 32 bits
    class LongLong; // at least 64 bits

    class Float;  // 32 bits
    class Double; // 64 bits
    class LongDouble;

    using Object::operator==;
    virtual bool operator==(const size_t &) = 0;
    virtual bool operator==(const short &) = 0;
    virtual bool operator==(const int &) = 0;
    virtual bool operator==(const long &) = 0;
    virtual bool operator==(const long long &) = 0;
    virtual bool operator==(const float &) = 0;
    virtual bool operator==(const double &) = 0;
    virtual bool operator==(const long double &) = 0;

    virtual bool operator<(const ref<Number> &) = 0;
    virtual bool operator>(const ref<Number> &) = 0;
    virtual bool operator<(const option<Number> &) = 0;
    virtual bool operator>(const option<Number> &) = 0;

    virtual bool operator<(const size_t &) = 0;
    virtual bool operator<(const short &) = 0;
    virtual bool operator<(const int &) = 0;
    virtual bool operator<(const long &) = 0;
    virtual bool operator<(const long long &) = 0;
    virtual bool operator<(const float &) = 0;
    virtual bool operator<(const double &) = 0;
    virtual bool operator<(const long double &) = 0;

    virtual bool operator>(const size_t &) = 0;
    virtual bool operator>(const short &) = 0;
    virtual bool operator>(const int &) = 0;
    virtual bool operator>(const long &) = 0;
    virtual bool operator>(const long long &) = 0;
    virtual bool operator>(const float &) = 0;
    virtual bool operator>(const double &) = 0;
    virtual bool operator>(const long double &) = 0;
};

template <typename T>
int Number::cmp(const size_t &s, const T &other)
{
    if (other < 0)
        return 1;

    auto unsignedNumber = static_cast<typename std::make_unsigned<T>::type>(other);
    if (s < unsignedNumber)
        return -1;
    else if (s > unsignedNumber)
        return 1;
    else
        return 0;
}

template <typename T>
int Number::cmp(const T &other, const size_t &s)
{
    if (other < 0)
        return -1;

    auto unsignedNumber = static_cast<typename std::make_unsigned<T>::type>(other);
    if (unsignedNumber < s)
        return -1;
    else if (unsignedNumber > s)
        return 1;
    else
        return 0;
}
