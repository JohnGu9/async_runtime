#pragma once
#include "option.h"
#include "ref.h"

namespace std
{
    template <typename T>
    struct hash<::option<T>>
    {
        std::size_t operator()(const ::option<T> &other) const
        {
            if (auto ptr = other.get())
            {
                return ptr->hashCode();
            }
            return 0;
        }
    };

    template <typename T>
    struct hash<::ref<T>>
    {
        std::size_t operator()(const ::ref<T> &other) const { return other->hashCode(); }
    };

    template <typename T>
    struct hash<::lateref<T>>
    {
        std::size_t operator()(const ::lateref<T> &other) const { return other->hashCode(); }
    };
} // namespace std
