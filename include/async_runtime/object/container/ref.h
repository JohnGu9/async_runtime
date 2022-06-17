#pragma once
#include "iterable.h"

template <typename T>
class ref<Iterable<T>> : public _async_runtime::RefImplement<Iterable<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Iterable<T>>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Iterable<T>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Iterable<T>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref<ConstIterator<T>> begin() const { return (*this)->begin(); }
    ref<ConstIterator<T>> end() const { return (*this)->end(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Iterable<T>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Iterable<T>, R>::value>::type * = nullptr>
    ref(std::shared_ptr<R> &&other) : super(std::move(other)) {}
};
