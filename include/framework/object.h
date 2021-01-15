#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <assert.h>
#include <unordered_map>
#include <vector>

#include "./basic/io.h"

using String = std::string;

class Object : public virtual std::enable_shared_from_this<Object>
{
public:
    using RuntimeType = String;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T>
    using WeakRef = std::weak_ptr<T>;

    template <typename Element>
    using List = std::vector<Element>;
    template <typename Key, typename Value>
    using Map = std::unordered_map<Key, Value>;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    inline static Ref<T> create(_Args &&...);
    template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    inline static Ref<T> create(_Args &&...);
    template <typename T0, typename T1>
    inline static bool identical(const Ref<T0> &, const Ref<T1> &);

    // static cast
    template <typename T, typename R, typename std::enable_if<std::is_base_of<Object, R>::value>::type * = nullptr>
    inline static Ref<T> cast(R *);

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
    inline static Ref<T> self(T *);

    // dynamic cast
    template <typename T>
    Ref<T> cast();

    template <typename T>
    bool is();

    virtual String toString();
    virtual void toStringStream(std::stringstream &);
    virtual RuntimeType runtimeType();

    virtual ~Object();
};

// http://www.enseignement.polytechnique.fr/informatique/INF478/docs/Cpp/en/cpp/memory/shared_ptr/operator_cmp.html
// C++11 already implement this operator

// template <typename T0, typename T1>
// inline bool operator==(Object::Ref<T0> object0, Object::Ref<T1> object1)
// {
//     return (size_t)(object0.get()) == (size_t)(object1.get());
// }

// template <typename T0, typename T1>
// inline bool operator!=(Object::Ref<T0> object0, Object::Ref<T1> object1)
// {
//     return (size_t)(object0.get()) != (size_t)(object1.get());
// }

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
inline Object::Ref<T> Object::create(_Args &&... __args)
{
    return std::make_shared<T>(__args...);
}

template <typename T, typename std::enable_if<!std::is_base_of<Object, T>::value>::type *, class... _Args>
inline Object::Ref<T> Object::create(_Args &&... __args)
{
    assert(warning_print("Object::create call on a not Object derived class [" << typeid(T).name() << "]"));
    return std::make_shared<T>(__args...);
}

template <typename T0, typename T1>
inline bool Object::identical(const Object::Ref<T0> &object0, const Object::Ref<T1> &object1)
{
    return (size_t)(object0.get()) == (size_t)(object1.get());
}

template <typename T, typename R, typename std::enable_if<std::is_base_of<Object, R>::value>::type *>
inline Object::Ref<T> Object::cast(R *other)
{
    return std::shared_ptr<T>(other->shared_from_this(), other);
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
inline Object::Ref<T> Object::self(T *other)
{
    return std::shared_ptr<T>(other->shared_from_this(), static_cast<T *>(other));
}

template <typename T>
inline Object::Ref<T> Object::cast()
{
    return std::dynamic_pointer_cast<T>(this->shared_from_this());
}

template <typename T>
inline bool Object::is()
{
    return dynamic_cast<T *>(this);
}

void print(Object::Ref<Object> object);