#pragma once

#include "declare.h"
#include <ostream>

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

    template <typename T0, typename T1,
              typename std::enable_if<std::is_base_of<Object, T0>::value>::type * = nullptr,
              typename std::enable_if<std::is_base_of<Object, T1>::value>::type * = nullptr>
    static bool equal(const option<T0> &, const option<T1> &);
    template <typename T0, typename T1,
              typename std::enable_if<std::is_base_of<Object, T0>::value>::type * = nullptr,
              typename std::enable_if<std::is_base_of<Object, T1>::value>::type * = nullptr>
    static bool equal(const ref<T0> &, const ref<T1> &);
    template <typename T0, typename T1,
              typename std::enable_if<std::is_base_of<Object, T0>::value>::type * = nullptr,
              typename std::enable_if<std::is_base_of<Object, T1>::value>::type * = nullptr>
    static bool equal(const ref<T0> &, const option<T1> &);
    template <typename T0, typename T1,
              typename std::enable_if<std::is_base_of<Object, T0>::value>::type * = nullptr,
              typename std::enable_if<std::is_base_of<Object, T1>::value>::type * = nullptr>
    static bool equal(const option<T0> &, const ref<T1> &);

    template <typename T>
    static bool isNull(const option<T> &) noexcept;
    template <typename T>
    static bool isNull(option<T> &&) noexcept;

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

    /**
     * @brief the function call by operator==(...) and Object::equal
     * override this function to custom your object compare
     *
     * @param other
     * @return true = consider [other] is equal to self
     * @return false = consider [other] is not equal to self
     */
    virtual bool operator==(ref<Object> other);

    /**
     * @brief the operator<<(os, object) underlay implement
     * not recommend to override the function
     * this function depend on Object::toStringStream
     * consider to override Object::toStringStream first
     *
     * @return ref<String>
     */
    virtual ref<String> toString();

    /**
     * @brief the Object::toString underlay implement
     */
    virtual void toStringStream(std::ostream &);

    /**
     * @brief get current object runtime type information
     * not recommend to override the function
     *
     * @return RuntimeType
     */
    virtual RuntimeType runtimeType();

    /**
     * @brief Destroy the Object object
     * under oop system, this's not recommend to custom the destructor
     * just only recommend use it under debug mode to do some resources-checking
     */
    virtual ~Object() {}

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
};
