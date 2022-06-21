#pragma once
#include "iterable.h"
#include "pair.h"

template <typename T>
class ref<Iterable<T>> : public _async_runtime::RefImplement<Iterable<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Iterable<T>>;
    using element_type = Iterable<T>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(const std::initializer_list<T> &list);
    ref(std::initializer_list<T> &&list);

    ref<ConstIterator<T>> begin() const { return (*this)->begin(); }
    ref<ConstIterator<T>> end() const { return (*this)->end(); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    ref(std::shared_ptr<R> &&other) : super(std::move(other)) {}
};

template <typename T>
class lateref<Iterable<T>> : public ref<Iterable<T>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<Iterable<T>>;
    using element_type = Iterable<T>;

public:
    explicit lateref() : super() {}
    lateref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<element_type, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : super(std::move(other)) {}

    lateref(const std::initializer_list<T> &list) : super(list) {}
    lateref(std::initializer_list<T> &&list) : super(std::move(list)) {}
};
