#pragma once

#include <memory>
#include <sstream>
#include <exception>
#include <assert.h>
#include "basic/io.h"
#include "basic/ref.h"

// native copyable type that don't not need ref
class String;
// native copyable type that don't not need ref
class Duration;
// native copyable type that don't not need ref
template <typename T = std::nullptr_t>
class Function;
// native copyable type that don't not need ref
template <typename Key, typename Value>
class Map;
// native copyable type that don't not need ref
template <typename T>
class Set;
// native copyable type that don't not need ref
template <typename T>
class List;

#define self() Object::cast<>(this)

class Object : public std::enable_shared_from_this<Object>
{
    ref<Object> shared_from_this() { return ref<Object>(std::enable_shared_from_this<Object>::shared_from_this()); }

public:
    using RuntimeType = String;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static void detach(ref<T> &);

    template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    static ref<T> create(_Args &&...);
    template <typename T0, typename T1>
    static bool identical(const option<T0> &, const option<T1> &);

    // static cast
    template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    static ref<T> cast(R *);
    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    static ref<T> cast(T *);

    // dynamic cast
    template <typename T>
    option<T> cast();
    template <typename T>
    ref<T> covariant();

    virtual String toString();
    virtual void toStringStream(std::ostream &);
    virtual RuntimeType runtimeType();
    virtual void init() {}

    Object() {}
    virtual ~Object() {}

    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;
};

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
ref<T> Object::create(_Args &&...__args)
{
    finalref<T> object = std::make_shared<T>(std::forward<_Args>(__args)...);
    object->init();
    return object;
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
void Object::detach(ref<T> &object)
{
    static_cast<std::shared_ptr<T>>(object) = nullptr;
}

template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type *, class... _Args>
ref<T> Object::create(_Args &&...__args)
{
    // recommend set a break pointer here
    // not-Object class should not create from this api
    assert(debug_print("Object::create call on a not Object derived class [" << typeid(T).name() << "]"));
    return std::make_shared<T>(std::forward<_Args>(__args)...);
}

template <typename T0, typename T1>
bool Object::identical(const option<T0> &object0, const option<T1> &object1)
{
    return (size_t)(object0.get()) == (size_t)(object1.get());
}

template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
ref<T> Object::cast(R *other)
{
    return std::shared_ptr<T>(other->shared_from_this(), static_cast<T *>(other));
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
ref<T> Object::cast(T *other)
{
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
        throw std::runtime_error(std::string("Invail type covariant from [") + typeid(*this).name() + "] to [" + typeid(T).name() + "]");
}

void print(option<Object> object);

#include "basic/string.h"
#include "basic/duration.h"
#include "basic/function.h"
#include "basic/container.h"