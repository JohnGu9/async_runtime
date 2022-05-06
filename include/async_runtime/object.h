#pragma once

#include <assert.h>
#include <exception>
#include <sstream>

#include "basic/ref.h"

#define self() Object::cast<>(this)

class String;

/**
 * @brief
 * Object is the element of nullsafety system
 * Only Object can take advantage of nullsafety system
 *
 * @example
 * class YourClass : public virtual Objectl Object{};
 * ref<YourClass> yourClass = Object::create<YourClass>();
 *
 */
class Object : public std::enable_shared_from_this<Object>
{
    ref<Object> shared_from_this() { return ref<Object>(std::enable_shared_from_this<Object>::shared_from_this()); }

public:
    using RuntimeType = size_t;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static void detach(ref<T> &);

    template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);

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

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
ref<T> Object::create(_Args &&...__args)
{
    finalref<T> object = std::make_shared<T>(std::forward<_Args>(__args)...);
    object->init();
    return object;
}

/**
 * @brief detach let ref force to release the refences of object
 * object may not dispose if there is any other ref of itself
 * be careful to use this function that may cause serious problem
 *
 * @tparam T
 * @tparam *
 * @param object the ref need to force to release
 */
template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
void Object::detach(ref<T> &object)
{
    std::shared_ptr<T> &obj = object;
    obj = nullptr;
}

#ifndef NDEBUG
#include <iostream>
#endif

template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type *, class... _Args>
ref<T> Object::create(_Args &&...__args)
{
    // recommend set a break pointer here
    // not-Object class should not create from this api

#ifndef NDEBUG
    std::cout << "Object::create call on a not Object derived class [" << typeid(T).name() << "]" << std::endl;
#endif
    return std::make_shared<T>(std::forward<_Args>(__args)...);
}

template <typename T0, typename T1>
bool Object::identical(const option<T0> &lhs, const option<T1> &rhs)
{
    return (size_t)(lhs.get()) == (size_t)(rhs.get());
}

template <typename T0, typename T1>
bool Object::identical(const ref<T0> &lhs, const ref<T1> &rhs)
{
    return (size_t)(lhs.get()) == (size_t)(rhs.get());
}

template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
ref<T> Object::cast(R *other)
{
    assert(other != nullptr);
    return std::shared_ptr<T>(other->shared_from_this(), static_cast<T *>(other));
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
ref<T> Object::cast(T *other)
{
    assert(other != nullptr);
    return std::shared_ptr<T>(other->shared_from_this(), other);
}

template <typename T>
option<T> Object::cast()
{
    return std::dynamic_pointer_cast<T>(this->shared_from_this());
}

template <typename T>
ref<T> Object::covariant()
{
    if (T *castedPointer = dynamic_cast<T *>(this))
        return Object::cast<>(castedPointer);
    else
    {
        std::stringstream ss;
        ss << "Invalid type covariant from [" << typeid(*this).name() << "] to [" << typeid(T).name() << "]";
        throw std::runtime_error(ss.str());
    }
}

/**
 * @brief object.h come with [function.h] [string.h] [container.h]
 * that you do not need to include them alone
 * include them alone may cause include order issue
 *
 */
#include "basic/container.h"
#include "basic/function.h"
#include "basic/string.h"
