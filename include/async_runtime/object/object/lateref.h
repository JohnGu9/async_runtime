#pragma once
#include "ref.h"

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    explicit lateref() : ref<T>() {}
    lateref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<T>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : ref<T>(std::move(other)) {}
};
