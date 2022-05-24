#pragma once

#include <assert.h>
#include <exception>
#include <sstream>

#include "declare.h"

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
    ref<Object> shared_from_this();

public:
    using RuntimeType = size_t;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static void detach(ref<T> &);

    template <typename T0, typename T1>
    static bool identical(const option<T0> &, const option<T1> &);
    template <typename T0, typename T1>
    static bool identical(const ref<T0> &, const ref<T1> &);

    // static cast
    template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    static ref<T> cast(R *);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static ref<T> cast(T *);

    // dynamic cast
    template <typename T>
    option<T> cast(); // safely cast
    template <typename T>
    ref<T> covariant(); // unsafely cast

    virtual ref<String> toString();
    virtual void toStringStream(std::ostream &);
    virtual RuntimeType runtimeType();
    virtual ~Object() {}

protected:
    Object() {}
    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;

    // @mustCallSuper
    virtual void init() {}
};
