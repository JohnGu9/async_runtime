#pragma once
#include "option.h"
#include "ref.h"

namespace std
{
    template <typename T>
    struct hash<::option<T>>
    {
        std::size_t operator()(const ::option<T> &other) const { return other.hashCode(); }
    };

    template <typename T>
    struct hash<::ref<T>>
    {
        std::size_t operator()(const ::ref<T> &other) const
        {
            static const auto hs = hash<::option<T>>();
            return hs(static_cast<const ::option<T> &>(other));
        }
    };
} // namespace std

namespace _async_runtime
{
    inline size_t hash_c_string(const char *p, size_t s)
    {
        static const size_t prime = 31;
        size_t result = 0;
        for (size_t i = 0; i < s; ++i)
        {
            result = p[i] + (result * prime);
        }
        return result;
    }
}

namespace std
{
    template <>
    struct hash<::ref<String>>
    {
        std::size_t operator()(const ::ref<String> &other) const
        {
            return ::_async_runtime::hash_c_string(other->data(), other->length());
        }
    };

    template <>
    struct hash<::option<String>>
    {
        std::size_t operator()(const ::option<String> &other) const
        {
            static const auto nullResult = hash<std::shared_ptr<String>>()(nullptr);
            auto ptr = other.get();
            if (ptr != nullptr)
            {
                return ::_async_runtime::hash_c_string(ptr->data(), ptr->length());
            }
            return nullResult;
        }
    };
} // namespace std
