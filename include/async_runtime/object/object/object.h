#pragma once

#include "declare.h"
#include <sstream>

/**
 * @brief
 * Object is the element of nullsafety system
 * Only Object can take advantage of nullsafety system
 *
 * @example
 * class YourClass : public virtual Object{};
 * ref<YourClass> yourClass = Object::create<YourClass>();
 *
 */
class Object : public std::enable_shared_from_this<Object>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT;

public:
    using RuntimeType = size_t;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static void detach(ref<T> &) noexcept;

    template <typename T0, typename T1>
    static bool identical(const option<T0> &, const option<T1> &) noexcept;
    template <typename T0, typename T1>
    static bool identical(const ref<T0> &, const ref<T1> &) noexcept;
    template <typename T0, typename T1>
    static bool identical(const ref<T0> &, const option<T1> &) noexcept;
    template <typename T0, typename T1>
    static bool identical(const option<T0> &, const ref<T1> &) noexcept;

    template <typename T0, typename T1>
    static bool equal(const option<T0> &, const option<T1> &);
    template <typename T0, typename T1>
    static bool equal(const ref<T0> &, const ref<T1> &);
    template <typename T0, typename T1>
    static bool equal(const ref<T0> &, const option<T1> &);
    template <typename T0, typename T1>
    static bool equal(const option<T0> &, const ref<T1> &);

    template <typename T>
    static bool isNull(const option<T> &) noexcept;

    // static cast
    template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    static ref<T> cast(R *);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static ref<T> cast(T *);

    Object() {}
    // @mustCallSuper
    virtual void init() {}

    // dynamic cast
    template <typename T>
    option<T> cast() noexcept; // safely cast
    template <typename T>
    ref<T> covariant() noexcept(false); // unsafely cast

    virtual bool operator==(ref<Object> other);
    virtual ref<String> toString();
    virtual void toStringStream(std::ostream &);
    virtual RuntimeType runtimeType();
    virtual ~Object() {}

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
};
