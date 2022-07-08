#pragma once

#include "../../object/object.h"

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

    class SizeType;

    class Short;    // at least 16 bits
    class Integer;  // at least 16 bits
    class Long;     // at least 32 bits
    class LongLong; // at least 64 bits

    class Float;  // 32 bits
    class Double; // 64 bits

    using Object::operator==;
    virtual bool operator==(const size_t &) = 0;
    virtual bool operator==(const short &) = 0;
    virtual bool operator==(const int &) = 0;
    virtual bool operator==(const long &) = 0;
    virtual bool operator==(const long long &) = 0;
    virtual bool operator==(const float &) = 0;
    virtual bool operator==(const double &) = 0;

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

    virtual bool operator>(const size_t &) = 0;
    virtual bool operator>(const short &) = 0;
    virtual bool operator>(const int &) = 0;
    virtual bool operator>(const long &) = 0;
    virtual bool operator>(const long long &) = 0;
    virtual bool operator>(const float &) = 0;
    virtual bool operator>(const double &) = 0;
};
