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
            if_not_null(other)
            {
                return other->hashCode();
            }
            end_if();
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

namespace _async_runtime
{
    inline size_t hash_c_string(const char *p, size_t s)
    {
        static const size_t prime = 31;
        size_t result = 0;
        for (size_t i = 0; i < s; ++i)
            result = p[i] + (result * prime);
        return result;
    }
}
