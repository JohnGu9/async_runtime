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

    template <typename T>
    class If
    {
    public:
        template <typename Base>
        using isDerivedOf = typename std::enable_if<std::is_base_of<Base, T>::value>;
        template <typename Derived>
        using isBaseOf = typename std::enable_if<std::is_base_of<T, Derived>::value>;
    };

    template <typename T>
    class Derived
    {
    public:
        template <typename Base>
        using isDerivedOf = typename std::enable_if<std::is_base_of<Base, T>::value>;
    };

    template <typename T>
    class Base
    {
    public:
        template <typename Derived>
        using isBaseOf = typename std::enable_if<std::is_base_of<T, Derived>::value>;
    };

    /**
     * @brief enable template function that the template [T] is base of Object
     *
     * @usage typename isBaseOf<T>::type * = nullptr
     *
     * @equal If<T>::isDerivedOf<Object>
     * @equal If<Object>::isBaseOf<T>
     * @equal Base<Object>::isBaseOf<T>
     * @equal Derived<T>::isDerivedOf<Object>
     * @equal std::enable_if<std::is_base_of<Object T>::value>
     *
     * @tparam T
     */
    template <typename T>
    using isBaseOf = typename Base<Object>::isBaseOf<T>;

    template <typename T, typename isBaseOf<T>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);
    template <typename T, typename isBaseOf<T>::type * = nullptr>
    static void detach(ref<T> &) noexcept;

    template <typename T0, typename T1>
    static bool identical(const option<T0> &, const option<T1> &) noexcept;
    template <typename T0, typename T1>
    static bool identical(const ref<T0> &, const ref<T1> &) noexcept;
    template <typename T0, typename T1>
    static bool identical(const ref<T0> &, const option<T1> &) noexcept;
    template <typename T0, typename T1>
    static bool identical(const option<T0> &, const ref<T1> &) noexcept;

    template <typename T0, typename T1, typename Object::isBaseOf<T0>::type * = nullptr, typename Object::isBaseOf<T1>::type * = nullptr>
    static bool equal(const option<T0> &, const option<T1> &);
    template <typename T0, typename T1, typename Object::isBaseOf<T0>::type * = nullptr, typename Object::isBaseOf<T1>::type * = nullptr>
    static bool equal(const ref<T0> &, const ref<T1> &);
    template <typename T0, typename T1, typename Object::isBaseOf<T0>::type * = nullptr, typename Object::isBaseOf<T1>::type * = nullptr>
    static bool equal(const ref<T0> &, const option<T1> &);
    template <typename T0, typename T1, typename Object::isBaseOf<T0>::type * = nullptr, typename Object::isBaseOf<T1>::type * = nullptr>
    static bool equal(const option<T0> &, const ref<T1> &);

    template <typename T>
    static bool isNull(const option<T> &) noexcept;

    // static cast
    template <typename T, typename R, typename Base<T>::template isBaseOf<R>::type * = nullptr>
    static ref<T> cast(R *);
    template <typename T, typename isBaseOf<T>::type * = nullptr>
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
