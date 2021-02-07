#pragma once

#include <memory>
#include <sstream>
#include <assert.h>
#include "basic/io.h"

// native copyable type that don't not need Object::Ref
class String;
// native copyable type that don't not need Object::Ref
class Duration;
// native copyable type that don't not need Object::Ref
template <typename T = std::nullptr_t>
class Function;
// native copyable type that don't not need Object::Ref
template <typename Key, typename Value>
class Map;
// native copyable type that don't not need Object::Ref
template <typename T>
class Set;
// native copyable type that don't not need Object::Ref
template <typename T>
class List;

class Object : public std::enable_shared_from_this<Object>
{
public:
    using RuntimeType = String;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T>
    using WeakRef = std::weak_ptr<T>;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static Ref<T> create(_Args &&...);
    template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static Ref<T> create(_Args &&...);
    template <typename T0, typename T1>
    static bool identical(const Ref<T0> &, const Ref<T1> &);

    // static cast
    template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    static Ref<T> cast(R *);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static Ref<T> cast(T *);

    // dynamic cast
    template <typename T>
    Ref<T> cast();
    template <typename T>
    Ref<T> covariant();

    virtual String toString();
    virtual void toStringStream(std::ostream &);
    virtual RuntimeType runtimeType();

    Object() {}
    virtual ~Object() {}

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
};

#define Self Object::cast<>(this)

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
Object::Ref<T> Object::create(_Args &&...__args)
{
    return std::make_shared<T>(std::forward<_Args>(__args)...);
}

template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type *, class... _Args>
Object::Ref<T> Object::create(_Args &&...__args)
{
    // recommend set a break pointer here
    // not-Object class should not create from this api
    assert(debug_print("Object::create call on a not Object derived class [" << typeid(T).name() << "]"));
    return std::make_shared<T>(std::forward<_Args>(__args)...);
}

template <typename T0, typename T1>
bool Object::identical(const Object::Ref<T0> &object0, const Object::Ref<T1> &object1)
{
    return (size_t)(object0.get()) == (size_t)(object1.get());
}

template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
Object::Ref<T> Object::cast(R *other)
{
    return std::shared_ptr<T>(other->shared_from_this(), static_cast<T *>(other));
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
Object::Ref<T> Object::cast(T *other)
{
    return std::shared_ptr<T>(other->shared_from_this(), other);
}

template <typename T>
Object::Ref<T> Object::cast()
{
    return std::dynamic_pointer_cast<T>(this->shared_from_this());
}

template <typename T>
Object::Ref<T> Object::covariant()
{
    if (T *castedPointer = dynamic_cast<T *>(this))
        return Object::cast<>(castedPointer);
    else
        assert(false && "Covariant failed. ");
}

void print(Object::Ref<Object> object);

#include "basic/string.h"
#include "basic/duration.h"
#include "basic/function.h"
#include "basic/container.h"