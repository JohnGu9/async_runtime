#pragma once
#include "number_basic.h"

template <typename T>
bool Number::Basic<T>::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<Number *>(other.get()))
    {
        switch (ptr->type())
        {
        case Number::Type::SizeType:
            return this->operator==(dynamic_cast<Number::Basic<size_t> *>(ptr)->value);
        case Number::Type::Short:
            return this->operator==(dynamic_cast<Number::Basic<short> *>(ptr)->value);
        case Number::Type::Integer:
            return this->operator==(dynamic_cast<Number::Basic<int> *>(ptr)->value);
        case Number::Type::Long:
            return this->operator==(dynamic_cast<Number::Basic<long> *>(ptr)->value);
        case Number::Type::LongLong:
            return this->operator==(dynamic_cast<Number::Basic<long long> *>(ptr)->value);
        case Number::Type::Float:
            return this->operator==(dynamic_cast<Number::Basic<float> *>(ptr)->value);
        case Number::Type::Double:
            return this->operator==(dynamic_cast<Number::Basic<double> *>(ptr)->value);
        case Number::Type::LongDouble:
            return this->operator==(dynamic_cast<Number::Basic<long double> *>(ptr)->value);
        default:
            break;
        }
    }
    return false;
}

template <typename T>
bool Number::Basic<T>::operator<(const ref<Number> &other)
{
    auto ptr = other.get();
    switch (ptr->type())
    {
    case Number::Type::SizeType:
        return this->operator<(dynamic_cast<Number::Basic<size_t> *>(ptr)->value);
    case Number::Type::Short:
        return this->operator<(dynamic_cast<Number::Basic<short> *>(ptr)->value);
    case Number::Type::Integer:
        return this->operator<(dynamic_cast<Number::Basic<int> *>(ptr)->value);
    case Number::Type::Long:
        return this->operator<(dynamic_cast<Number::Basic<long> *>(ptr)->value);
    case Number::Type::LongLong:
        return this->operator<(dynamic_cast<Number::Basic<long long> *>(ptr)->value);
    case Number::Type::Float:
        return this->operator<(dynamic_cast<Number::Basic<float> *>(ptr)->value);
    case Number::Type::Double:
        return this->operator<(dynamic_cast<Number::Basic<double> *>(ptr)->value);
    case Number::Type::LongDouble:
        return this->operator<(dynamic_cast<Number::Basic<long double> *>(ptr)->value);
    default:
        throw NotImplementedError();
    }
}

template <typename T>
bool Number::Basic<T>::operator>(const ref<Number> &other)
{
    auto ptr = other.get();
    switch (ptr->type())
    {
    case Number::Type::SizeType:
        return this->operator>(dynamic_cast<Number::Basic<size_t> *>(ptr)->value);
    case Number::Type::Short:
        return this->operator>(dynamic_cast<Number::Basic<short> *>(ptr)->value);
    case Number::Type::Integer:
        return this->operator>(dynamic_cast<Number::Basic<int> *>(ptr)->value);
    case Number::Type::Long:
        return this->operator>(dynamic_cast<Number::Basic<long> *>(ptr)->value);
    case Number::Type::LongLong:
        return this->operator>(dynamic_cast<Number::Basic<long long> *>(ptr)->value);
    case Number::Type::Float:
        return this->operator>(dynamic_cast<Number::Basic<float> *>(ptr)->value);
    case Number::Type::Double:
        return this->operator>(dynamic_cast<Number::Basic<double> *>(ptr)->value);
    case Number::Type::LongDouble:
        return this->operator>(dynamic_cast<Number::Basic<long double> *>(ptr)->value);
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
