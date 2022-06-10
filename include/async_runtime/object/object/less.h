#pragma once

#include "../string.h"
#include <cstring>

namespace std
{
    template <>
    struct less<String>
    {
        bool operator()(const String &lhs, const String &rhs) const
        {
            if (lhs.length() == rhs.length())
                return std::strncmp(lhs.data(), rhs.data(), lhs.length()) < 0;
            return lhs.length() < rhs.length();
        }
    };

    template <>
    struct less<::ref<String>>
    {
        bool operator()(const ::ref<String> &lhs, const ::ref<String> &rhs) const
        {
            static const auto ls = std::less<String>();
            return ls(*lhs.get(), *rhs.get());
        }
    };

    template <>
    struct less<::option<String>>
    {
        bool operator()(const ::option<String> &lhs, const ::option<String> &rhs) const
        {
            auto lhsPointer = lhs.get();
            auto rhsPointer = rhs.get();
            if (lhsPointer == nullptr && rhsPointer == nullptr)
            {
                return false;
                // if two is equal, return value must be [false]
                // return [true] will cause "invalid comparator" error
            }
            else if (lhsPointer != nullptr && rhsPointer == nullptr)
            {
                return false;
            }
            else if (lhsPointer == nullptr && rhsPointer != nullptr)
            {
                return true;
            }
            else
            {
                static const auto ls = std::less<String>();
                return ls(*lhsPointer, *rhsPointer);
            }
        }
    };
}