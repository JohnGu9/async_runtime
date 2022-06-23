#pragma once
#include "ref.h"

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<T>;

public:
    using ref<T>::ref;

    explicit lateref() : super() {}
    lateref(std::nullptr_t) = delete;

    // msvc something can't infer template type, so add two constructors below make code more suitable on windows
    lateref(const ref<T> &other) : super(other) {}
    lateref(ref<T> &&other) : super(std::move(other)) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : super(std::move(other)) {}
};
