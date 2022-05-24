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

namespace std
{
    template <>
    struct hash<::ref<String>>
    {
        std::size_t operator()(const ::ref<String> &other) const
        {
            static const auto hs = hash<std::string>();
            return hs(static_cast<std::string>(*other));
        }
    };

    template <>
    struct hash<::option<String>>
    {
        std::size_t operator()(const ::option<String> &other) const
        {
            lateref<String> str;
            if (other.isNotNull(str))
            {
                static const auto hs = hash<::ref<String>>();
                return hs(str);
            }
            else
            {
                static const auto result = hash<std::shared_ptr<String>>()(nullptr);
                return result;
            }
        }
    };
} // namespace std
