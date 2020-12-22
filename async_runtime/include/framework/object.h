#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <assert.h>

#include <unordered_map>
#include <list>

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

class Object : public virtual std::enable_shared_from_this<Object>
{
public:
    using RuntimeType = std::string;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T>
    using WeakRef = std::weak_ptr<T>;

    template <typename Element>
    using List = std::list<Element>;
    template <typename Key, typename Value>
    using Map = std::unordered_map<Key, Value>;

    template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr, class... _Args>
    inline static Ref<T> create(_Args &&... __args);
    template <typename T0, typename T1>
    inline static bool identical(const Ref<T0> &object0, const Ref<T1> &object1);
    template <typename T, typename R, typename std::enable_if<std::is_base_of<Object, R>::value>::type * = nullptr>
    inline static Ref<T> cast(R *other);

    template <typename T>
    Ref<T> cast();

    virtual std::string toString();
    virtual void toStringStream(std::stringstream &ss);
    virtual RuntimeType runtimeType();

    virtual ~Object();
};

template <typename T0, typename T1>
inline bool operator==(Object::Ref<T0> object0, Object::Ref<T1> object1)
{
    return (size_t)(object0.get()) == (size_t)(object1.get());
}

template <typename T0, typename T1>
inline bool operator!=(Object::Ref<T0> object0, Object::Ref<T1> object1)
{
    return (size_t)(object0.get()) != (size_t)(object1.get());
}

template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
inline Object::Ref<T> Object::create(_Args &&... __args)
{
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
    return std::shared_ptr<T>(other->shared_from_this(), static_cast<T *>(other));
}

template <typename T>
inline Object::Ref<T> Object::cast()
{
    return std::dynamic_pointer_cast<T>(this->shared_from_this());
}

inline Object::RuntimeType Object::runtimeType()
{
    return typeid(*this).name();
}
