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

    // linux can't infer correct template constructor that inherited from ref<T>
    // two constructor below is to patch the problem
    lateref(const ref<T> &other) : super(other) {}
    lateref(ref<T> &&other) : super(std::move(other)) {}
};
