#pragma once
#include "../object/object.h"

/**
 * @file boolean.h
 * @brief Boolean toolkits just for providing a convince way to convert a native bool type to a Object like bool type.
 * The target is not for performance, just for consistent in oop system.
 *
 */

class Boolean : public virtual Object
{
public:
    const bool value;
    Boolean(bool value) : value(value) {}

    size_t hashCode() override;
    void toStringStream(std::ostream &os) override;

    bool operator==(const option<Object> &other) override;
    bool operator==(const bool &) const;
};

template <>
class ref<Boolean> : public _async_runtime::RefImplement<Boolean>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Boolean>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Boolean, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Boolean, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    ref(bool value) : super(Object::create<Boolean>(value)) {}

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Boolean>::RefImplement;
};
